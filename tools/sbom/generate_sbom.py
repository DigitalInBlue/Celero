#!/usr/bin/env python3
#
# SPDX SBOM generator for Celero
#
# Copyright 2015-2026 John Farrier
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
"""Generate SPDX 2.3 SBOM documents for Celero.

Two flavors are produced, because they answer different questions:

  library  What a consumer links against: the celero library plus the
           operating-system libraries it needs at runtime.  gtest is
           deliberately absent -- it is test-only and never enters the
           installed artifact.  This document is platform-specific, so one
           is emitted per vcpkg triplet.

  source   What is in the source distribution: everything in `library`, plus
           the vcpkg-resolved test and build dependencies, the vendored
           third-party headers under experiments/, and the GitHub Actions
           that make up the build pipeline.

Dependency facts are read from vcpkg's own per-port `vcpkg.spdx.json` output
rather than re-derived, so resolved versions, upstream URLs and source
checksums come straight from the tool that performed the resolution.

The `created` timestamp defaults to the HEAD commit time (or SOURCE_DATE_EPOCH
when set) so that regenerating from the same commit yields a byte-identical
document.
"""

import argparse
import hashlib
import json
import os
import re
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path

GENERATOR_VERSION = "1.1.0"

PROJECT_NAME = "celero"
PROJECT_HOMEPAGE = "https://github.com/DigitalInBlue/Celero"
PROJECT_REPOSITORY = "https://github.com/DigitalInBlue/Celero.git"
PROJECT_LICENSE = "Apache-2.0"
PROJECT_COPYRIGHT = "Copyright 2015-2023 John Farrier"
PROJECT_SUPPLIER = "Person: John Farrier"
PROJECT_DESCRIPTION = "Celero is a C++ micro-benchmarking library to measure the performance of C++ code."

SPDX_TIME_FORMAT = "%Y-%m-%dT%H:%M:%SZ"

# Triplets for which a library SBOM is published, matching the platforms built
# by .github/workflows/cmake-multi-platform.yml.  Used when --pinned is given
# without an explicit --triplet, so that regenerating the committed documents
# is a single command.
DEFAULT_TRIPLETS = ["arm64-osx", "x64-linux", "x64-windows"]

# How each vcpkg port relates to the celero product.  Ports absent from this
# map are reported on stderr and conservatively treated as build dependencies,
# so that a newly added *runtime* dependency cannot be silently misfiled as
# build-only.
PORT_SCOPE = {
	# Consumed only by the celero-test target, which is gated behind
	# CELERO_ENABLE_TESTS (OFF by default) and never linked into the
	# installed library.
	"gtest": "TEST_DEPENDENCY_OF",
	"vcpkg-cmake": "BUILD_DEPENDENCY_OF",
	"vcpkg-cmake-config": "BUILD_DEPENDENCY_OF",
}

# Operating-system libraries named in the top-level CMakeLists.txt.  These are
# supplied by the platform, not vendored or fetched, so they carry no download
# location.
SYSTEM_LIBRARIES = {
	"windows": [
		{
			"name": "powrprof",
			"comment": (
				"Windows Power Profile API import library, linked via SYSLIBS in CMakeLists.txt. "
				"<powrprof.h> is included by src/Utilities.cpp; no power-management entry point is "
				"currently called, so this link may be vestigial."
			),
		},
		{
			"name": "psapi",
			"comment": (
				"Windows Process Status API import library, linked via SYSLIBS in CMakeLists.txt. "
				"Used by src/Memory.cpp via GetProcessMemoryInfo() to sample process memory counters."
			),
		},
	],
	"linux": [
		{
			"name": "pthread",
			"comment": (
				"POSIX threads, linked via SYSLIBS in CMakeLists.txt and required for std::thread. "
				"Provided by the system C library (glibc or musl); not vendored by Celero."
			),
		}
	],
	"osx": [
		{
			"name": "pthread",
			"comment": (
				"POSIX threads, linked via SYSLIBS in CMakeLists.txt and required for std::thread. "
				"Provided by libSystem on macOS; not vendored by Celero."
			),
		}
	],
}

# Third-party source copied into the tree.  No scanner infers provenance from a
# pasted file, so these are recorded by hand and their content is hashed at
# generation time.  Neither is linked into the installed library: both compile
# only when CELERO_ENABLE_EXPERIMENTS=ON.
VENDORED_COMPONENTS = [
	{
		"spdxid": "SPDXRef-Package-openscenegraph-ref-ptr",
		"name": "openscenegraph-ref_ptr",
		"path": "experiments/ExperimentCostSharedPtr/osg_ref_ptr.h",
		"versionInfo": "NOASSERTION",
		"downloadLocation": "https://github.com/openscenegraph/OpenSceneGraph",
		"homepage": "https://www.openscenegraph.org/",
		"licenseConcluded": "LicenseRef-OSGPL",
		"licenseDeclared": "LicenseRef-OSGPL",
		"copyrightText": "Copyright (C) 1998-2006 Robert Osfield",
		"supplier": "Person: Robert Osfield",
		"comment": (
			"Excerpt of osg::ref_ptr vendored as a single header for the ExperimentCostSharedPtr "
			"benchmark. Licensed under the OpenSceneGraph Public License (OSGPL), NOT under Celero's "
			"Apache-2.0. Compiled only when CELERO_ENABLE_EXPERIMENTS=ON and never linked into the "
			"installed celero library, so it is present in the source distribution only."
		),
	},
	{
		"spdxid": "SPDXRef-Package-benchmarklibstest-particles",
		"name": "benchmarkLibsTest-particles",
		"path": "experiments/ExperimentParticles/Particles.h",
		"versionInfo": "NOASSERTION",
		"downloadLocation": "https://github.com/fenbf/benchmarkLibsTest",
		"homepage": "https://github.com/fenbf/benchmarkLibsTest",
		"licenseConcluded": "NOASSERTION",
		"licenseDeclared": "NOASSERTION",
		"copyrightText": "NOASSERTION",
		"supplier": "NOASSERTION",
		"comment": (
			"Particle benchmark types derived from https://github.com/fenbf/benchmarkLibsTest, as "
			"noted in the file's first line. The file carries no license header and the upstream "
			"license has not been established, so the license is NOASSERTION rather than assumed. "
			"Compiled only when CELERO_ENABLE_EXPERIMENTS=ON and never linked into the installed "
			"celero library, so it is present in the source distribution only."
		),
	},
]

EXTRACTED_LICENSES = {
	"LicenseRef-OSGPL": {
		"licenseId": "LicenseRef-OSGPL",
		"name": "OpenSceneGraph Public License, Version 0.0",
		"extractedText": (
			"This library is open source and may be redistributed and/or modified under the terms of "
			"the OpenSceneGraph Public License (OSGPL) version 0.0 or (at your option) any later "
			"version. The full license is in the LICENSE file included with the OpenSceneGraph "
			"distribution, and on the openscenegraph.org website. This library is distributed in the "
			"hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty "
			"of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the OpenSceneGraph Public "
			"License for more details."
		),
		"seeAlsos": ["https://www.openscenegraph.org/index.php/about/license"],
		"comment": (
			"The OSGPL is a wxWindows-style relaxation of the LGPL and has no SPDX short identifier, "
			"so it is recorded here as an extracted license."
		),
	}
}

GITHUB_GIT_RE = re.compile(r"^git\+https://github\.com/([^/]+)/([^/@]+?)(?:\.git)?@(.+)$")
USES_RE = re.compile(r"^\s*(?:-\s+)?uses:\s*[\"']?([A-Za-z0-9._-]+/[A-Za-z0-9._/-]+)@([A-Za-z0-9._-]+)", re.MULTILINE)


class GeneratorError(RuntimeError):
	"""Raised when the SBOM cannot be produced accurately."""


def run_git(project_root, *args):
	try:
		result = subprocess.run(
			["git", *args],
			cwd=str(project_root),
			capture_output=True,
			text=True,
			check=False,
		)
	except OSError:
		return None
	if result.returncode != 0:
		return None
	return result.stdout.strip() or None


def read_project_version(project_root):
	manifest = project_root / "vcpkg.json"
	if not manifest.is_file():
		raise GeneratorError("vcpkg.json not found; cannot determine the Celero version.")
	version = json.loads(manifest.read_text(encoding="utf-8")).get("version")
	if not version:
		raise GeneratorError("vcpkg.json does not declare a \"version\" field.")
	return version


def resolve_timestamp(explicit, project_root):
	if explicit:
		return explicit

	epoch = os.environ.get("SOURCE_DATE_EPOCH")
	if epoch is None:
		epoch = run_git(project_root, "log", "-1", "--format=%ct")

	if epoch is not None:
		try:
			return datetime.fromtimestamp(int(epoch), timezone.utc).strftime(SPDX_TIME_FORMAT)
		except (TypeError, ValueError):
			pass

	return datetime.now(timezone.utc).strftime(SPDX_TIME_FORMAT)


def sha256_file(path):
	digest = hashlib.sha256()
	with path.open("rb") as handle:
		for chunk in iter(lambda: handle.read(65536), b""):
			digest.update(chunk)
	return digest.hexdigest()


def purl_from_download(download_location, name, version):
	"""Build a package URL, preferring a precise github purl when possible."""
	match = GITHUB_GIT_RE.match(download_location or "")
	if match:
		owner, repo, ref = match.groups()
		return "pkg:github/{}/{}@{}".format(owner.lower(), repo.lower(), ref)

	purl = "pkg:generic/{}".format(name.lower())
	if version and version != "NOASSERTION":
		purl += "@{}".format(version)
	if download_location and download_location not in ("NONE", "NOASSERTION"):
		purl += "?download_url={}".format(download_location)
	return purl


def purl_external_ref(purl):
	return {
		"referenceCategory": "PACKAGE-MANAGER",
		"referenceType": "purl",
		"referenceLocator": purl,
	}


def spdx_id_for(prefix, name):
	"""SPDX identifiers permit only letters, digits, '.' and '-'."""
	sanitized = re.sub(r"[^A-Za-z0-9.\-]", "-", name)
	return "SPDXRef-{}-{}".format(prefix, sanitized)


def discover_vcpkg_spdx(project_root, explicit_root, triplet):
	"""Locate vcpkg's per-port SPDX documents.

	vcpkg writes these to <install_root>/<triplet>/share/<port>/vcpkg.spdx.json.
	The root lands next to the manifest for a bare `vcpkg install`, inside the
	CMake binary directory for a manifest-mode configure, and under the vcpkg
	checkout itself when the caller passes --x-install-root (as CI's vcpkg-action
	does), so all three are searched unless an explicit root is given.
	"""
	if explicit_root:
		roots = [Path(explicit_root)]
		if not roots[0].is_dir():
			raise GeneratorError("--vcpkg-installed path does not exist: {}".format(explicit_root))
	else:
		roots = [
			candidate
			for candidate in (
				project_root / "vcpkg_installed",
				project_root / "build" / "vcpkg_installed",
				project_root / "out" / "build" / "vcpkg_installed",
				project_root / "vcpkg" / "installed",
			)
			if candidate.is_dir()
		]

	documents = {}
	for root in roots:
		for triplet_dir in sorted(root.iterdir()):
			if not triplet_dir.is_dir() or triplet_dir.name == "vcpkg":
				continue
			if triplet and triplet_dir.name != triplet:
				continue
			for spdx_path in sorted(triplet_dir.glob("share/*/vcpkg.spdx.json")):
				documents.setdefault(triplet_dir.name, []).append(spdx_path)

	return documents


def parse_vcpkg_spdx(path):
	"""Reduce one vcpkg SPDX document to the facts worth republishing."""
	document = json.loads(path.read_text(encoding="utf-8"))
	packages = {package.get("SPDXID"): package for package in document.get("packages", [])}

	port = packages.get("SPDXRef-port")
	if port is None:
		return None

	resources = [
		package
		for spdx_id, package in sorted(packages.items())
		if spdx_id.startswith("SPDXRef-resource-")
	]

	primary = resources[0] if resources else {}
	checksums = [
		checksum
		for checksum in primary.get("checksums", [])
		if checksum.get("algorithm") and checksum.get("checksumValue")
	]

	return {
		"name": port.get("name", path.parent.name),
		"version": port.get("versionInfo", "NOASSERTION"),
		"license": port.get("licenseConcluded") or "NOASSERTION",
		"homepage": port.get("homepage"),
		"description": port.get("description"),
		"port_download": port.get("downloadLocation", "NOASSERTION"),
		"upstream_download": primary.get("downloadLocation") or port.get("downloadLocation") or "NOASSERTION",
		"upstream_name": primary.get("name"),
		"checksums": checksums,
		"extra_resources": [resource.get("downloadLocation") for resource in resources[1:]],
		"source": path,
	}


def collect_vcpkg_packages(spdx_paths, warnings):
	"""Turn vcpkg's SPDX output into SPDX packages plus their relationship scope.

	Only facts that hold for every platform are republished.  vcpkg's per-port
	ABI hash is deliberately excluded: it varies with triplet, toolchain and
	vcpkg revision, so recording it would make the source document -- which is
	generated from whichever single triplet happens to be installed -- differ
	between a developer's machine and CI even though the dependency set is
	identical.
	"""
	collected = []
	for path in spdx_paths:
		parsed = parse_vcpkg_spdx(path)
		if parsed is None:
			warnings.append("Skipped {}: no SPDXRef-port package found.".format(path))
			continue

		name = parsed["name"]
		scope = PORT_SCOPE.get(name)
		if scope is None:
			scope = "BUILD_DEPENDENCY_OF"
			warnings.append(
				"vcpkg port '{}' is not classified in PORT_SCOPE; defaulting to BUILD_DEPENDENCY_OF. "
				"Add it to tools/sbom/generate_sbom.py if it is a runtime or test dependency.".format(name)
			)

		comment_parts = [
			"Resolved by vcpkg from port {}@{}.".format(name, parsed["version"]),
			"Port recipe: {}.".format(parsed["port_download"]),
		]
		if parsed["extra_resources"]:
			comment_parts.append(
				"Additional upstream resources: {}.".format(", ".join(filter(None, parsed["extra_resources"])))
			)
		if scope == "TEST_DEPENDENCY_OF":
			comment_parts.append(
				"Test-only: consumed by the celero-test target behind CELERO_ENABLE_TESTS and not "
				"linked into the installed celero library."
			)

		package = {
			"SPDXID": spdx_id_for("Package", name),
			"name": name,
			"versionInfo": parsed["version"],
			"downloadLocation": parsed["upstream_download"],
			"filesAnalyzed": False,
			"licenseConcluded": parsed["license"],
			"licenseDeclared": parsed["license"],
			"copyrightText": "NOASSERTION",
			"supplier": "NOASSERTION",
			"primaryPackagePurpose": "LIBRARY",
			"comment": " ".join(comment_parts),
			"externalRefs": [
				purl_external_ref(purl_from_download(parsed["upstream_download"], name, parsed["version"]))
			],
		}

		if parsed["homepage"]:
			package["homepage"] = parsed["homepage"]
		if parsed["description"]:
			package["description"] = parsed["description"]
		if parsed["checksums"]:
			package["checksums"] = parsed["checksums"]

		collected.append((package, scope))

	collected.sort(key=lambda item: item[0]["name"])
	return collected


def collect_workflow_actions(project_root, warnings):
	"""Record the GitHub Actions that constitute the build pipeline."""
	workflow_dir = project_root / ".github" / "workflows"
	if not workflow_dir.is_dir():
		warnings.append("No .github/workflows directory; the build pipeline is absent from the SBOM.")
		return []

	found = {}
	for workflow in sorted(workflow_dir.glob("*.y*ml")):
		text = workflow.read_text(encoding="utf-8")
		for repository, ref in USES_RE.findall(text):
			found.setdefault((repository, ref), set()).add(workflow.name)

	packages = []
	for (repository, ref), workflows in sorted(found.items()):
		owner = repository.split("/", 1)[0]
		package = {
			"SPDXID": spdx_id_for("Package-action", "{}-{}".format(repository, ref)),
			"name": repository,
			"versionInfo": ref,
			"downloadLocation": "git+https://github.com/{}.git@{}".format("/".join(repository.split("/")[:2]), ref),
			"filesAnalyzed": False,
			"licenseConcluded": "NOASSERTION",
			"licenseDeclared": "NOASSERTION",
			"copyrightText": "NOASSERTION",
			"supplier": "Organization: {}".format(owner),
			"primaryPackagePurpose": "APPLICATION",
			"comment": "GitHub Action used by {}. Referenced by mutable tag '{}'.".format(
				", ".join(sorted(workflows)), ref
			),
			"externalRefs": [
				purl_external_ref("pkg:githubactions/{}@{}".format(repository.lower(), ref))
			],
		}
		packages.append(package)

	if not packages:
		warnings.append("No 'uses:' entries found in .github/workflows; the build pipeline is absent from the SBOM.")

	return packages


def collect_vendored_packages(project_root):
	"""Hash and describe the third-party source copied into the tree."""
	packages = []
	for component in VENDORED_COMPONENTS:
		path = project_root / component["path"]
		if not path.is_file():
			raise GeneratorError(
				"Vendored component '{}' is declared in tools/sbom/generate_sbom.py but "
				"{} does not exist. Update VENDORED_COMPONENTS so the SBOM stays truthful.".format(
					component["name"], component["path"]
				)
			)

		package = {
			"SPDXID": component["spdxid"],
			"name": component["name"],
			"versionInfo": component["versionInfo"],
			"downloadLocation": component["downloadLocation"],
			"homepage": component["homepage"],
			"filesAnalyzed": False,
			"licenseConcluded": component["licenseConcluded"],
			"licenseDeclared": component["licenseDeclared"],
			"copyrightText": component["copyrightText"],
			"supplier": component["supplier"],
			"primaryPackagePurpose": "SOURCE",
			"sourceInfo": "Vendored at {} within the Celero source tree.".format(component["path"]),
			"comment": component["comment"],
			"checksums": [{"algorithm": "SHA256", "checksumValue": sha256_file(path)}],
		}
		packages.append(package)

	return packages


def celero_package(version, reference, commit, purpose_comment):
	download = "git+{}@{}".format(PROJECT_REPOSITORY, reference)

	package = {
		"SPDXID": "SPDXRef-Package-celero",
		"name": PROJECT_NAME,
		"versionInfo": version,
		"downloadLocation": download,
		"homepage": PROJECT_HOMEPAGE,
		"filesAnalyzed": False,
		"licenseConcluded": PROJECT_LICENSE,
		"licenseDeclared": PROJECT_LICENSE,
		"copyrightText": PROJECT_COPYRIGHT,
		"supplier": PROJECT_SUPPLIER,
		"description": PROJECT_DESCRIPTION,
		"primaryPackagePurpose": "LIBRARY",
		"comment": purpose_comment,
		"externalRefs": [
			purl_external_ref("pkg:github/digitalinblue/celero@{}".format(reference))
		],
	}

	# Recorded only when the document is pinned to the working-tree commit. A
	# document pinned to a release tag must not carry an incidental HEAD SHA:
	# that would change on every unrelated commit and make a committed SBOM
	# churn without any dependency having changed.
	if commit and commit == reference:
		package["sourceInfo"] = "Built from {} at commit {}.".format(PROJECT_REPOSITORY, commit)

	return package


def platform_for_triplet(triplet):
	lowered = triplet.lower()
	if "windows" in lowered or "uwp" in lowered or "mingw" in lowered:
		return "windows"
	if "osx" in lowered or "ios" in lowered:
		return "osx"
	return "linux"


def system_library_packages(triplet):
	packages = []
	for library in SYSTEM_LIBRARIES[platform_for_triplet(triplet)]:
		packages.append(
			{
				"SPDXID": spdx_id_for("Package-syslib", library["name"]),
				"name": library["name"],
				"versionInfo": "NOASSERTION",
				"downloadLocation": "NOASSERTION",
				"filesAnalyzed": False,
				"licenseConcluded": "NOASSERTION",
				"licenseDeclared": "NOASSERTION",
				"copyrightText": "NOASSERTION",
				"supplier": "NOASSERTION",
				"primaryPackagePurpose": "LIBRARY",
				"comment": library["comment"] + " Supplied by the operating system; not distributed with Celero.",
			}
		)
	return packages


def build_document(name, namespace, timestamp, packages, relationships, extracted_licenses=None):
	document = {
		"spdxVersion": "SPDX-2.3",
		"dataLicense": "CC0-1.0",
		"SPDXID": "SPDXRef-DOCUMENT",
		"name": name,
		"documentNamespace": namespace,
		"creationInfo": {
			"created": timestamp,
			"creators": [
				"Tool: celero-sbom-generator-{}".format(GENERATOR_VERSION),
				PROJECT_SUPPLIER,
			],
			"comment": (
				"Generated by tools/sbom/generate_sbom.py. Dependency versions, upstream URLs and "
				"source checksums are read from vcpkg's per-port SPDX output; vendored third-party "
				"headers are declared explicitly and hashed at generation time."
			),
		},
		"documentDescribes": ["SPDXRef-Package-celero"],
		"packages": packages,
		"relationships": relationships,
	}

	if extracted_licenses:
		document["hasExtractedLicensingInfos"] = extracted_licenses

	return document


def make_namespace(flavor, version, reference, triplet=None):
	suffix = reference or "unversioned"
	parts = [PROJECT_HOMEPAGE, "spdxdocs", "celero", flavor]
	if triplet:
		parts.append(triplet)
	parts.append("{}-{}".format(version, suffix))
	return "/".join(parts)


def generate_library_document(project_root, version, reference, commit, timestamp, triplet):
	packages = [
		celero_package(
			version,
			reference,
			commit,
			"The installed Celero library. Celero has no third-party runtime dependencies; only the "
			"operating-system libraries listed in this document are required at link and run time. "
			"gtest is intentionally absent because it is test-only and never enters the installed "
			"artifact -- see the source-distribution SBOM for the full build-time inventory.",
		)
	]

	relationships = [
		{
			"spdxElementId": "SPDXRef-DOCUMENT",
			"relationshipType": "DESCRIBES",
			"relatedSpdxElement": "SPDXRef-Package-celero",
		}
	]

	for package in system_library_packages(triplet):
		packages.append(package)
		relationships.append(
			{
				"spdxElementId": "SPDXRef-Package-celero",
				"relationshipType": "DEPENDS_ON",
				"relatedSpdxElement": package["SPDXID"],
			}
		)

	return build_document(
		"celero-{}-{}".format(version, triplet),
		make_namespace("library", version, reference, triplet),
		timestamp,
		packages,
		relationships,
	)


def generate_source_document(project_root, version, reference, commit, timestamp, vcpkg_paths, warnings):
	packages = [
		celero_package(
			version,
			reference,
			commit,
			"The Celero source distribution. This document covers everything present in the "
			"repository: the library itself, the vcpkg-resolved test and build dependencies, the "
			"third-party headers vendored under experiments/, and the GitHub Actions that make up "
			"the build pipeline. For what a consumer actually links against, see the per-triplet "
			"library SBOM instead.",
		)
	]

	relationships = [
		{
			"spdxElementId": "SPDXRef-DOCUMENT",
			"relationshipType": "DESCRIBES",
			"relatedSpdxElement": "SPDXRef-Package-celero",
		}
	]

	for package, scope in collect_vcpkg_packages(vcpkg_paths, warnings):
		packages.append(package)
		relationships.append(
			{
				"spdxElementId": package["SPDXID"],
				"relationshipType": scope,
				"relatedSpdxElement": "SPDXRef-Package-celero",
			}
		)

	for package in collect_vendored_packages(project_root):
		packages.append(package)
		relationships.append(
			{
				"spdxElementId": "SPDXRef-Package-celero",
				"relationshipType": "CONTAINS",
				"relatedSpdxElement": package["SPDXID"],
			}
		)

	for package in collect_workflow_actions(project_root, warnings):
		packages.append(package)
		relationships.append(
			{
				"spdxElementId": package["SPDXID"],
				"relationshipType": "BUILD_TOOL_OF",
				"relatedSpdxElement": "SPDXRef-Package-celero",
			}
		)

	used_licenses = {package.get("licenseConcluded") for package in packages}
	used_licenses |= {package.get("licenseDeclared") for package in packages}
	extracted = [
		EXTRACTED_LICENSES[license_id]
		for license_id in sorted(EXTRACTED_LICENSES)
		if license_id in used_licenses
	]

	return build_document(
		"celero-{}-source".format(version),
		make_namespace("source", version, reference),
		timestamp,
		packages,
		relationships,
		extracted,
	)


def write_document(document, path):
	path.parent.mkdir(parents=True, exist_ok=True)
	path.write_text(json.dumps(document, indent=2, sort_keys=False) + "\n", encoding="utf-8")
	return path


def parse_args(argv):
	parser = argparse.ArgumentParser(
		description="Generate SPDX 2.3 SBOM documents for Celero.",
		formatter_class=argparse.RawDescriptionHelpFormatter,
	)
	parser.add_argument(
		"--flavor",
		choices=["library", "source", "both"],
		default="both",
		help="Which document(s) to emit (default: both).",
	)
	parser.add_argument(
		"--project-root",
		default=str(Path(__file__).resolve().parents[2]),
		help="Root of the Celero checkout (default: inferred from this script's location).",
	)
	parser.add_argument(
		"--vcpkg-installed",
		default=None,
		help="Path to a vcpkg_installed directory. Auto-discovered when omitted.",
	)
	parser.add_argument(
		"--triplet",
		default=None,
		help="Restrict generation to a single vcpkg triplet (e.g. x64-linux).",
	)
	parser.add_argument(
		"--output-dir",
		default=None,
		help="Directory to write SBOM documents to (default: <project-root>/sbom).",
	)
	parser.add_argument(
		"--version",
		default=None,
		help="Override the Celero version (default: read from vcpkg.json).",
	)
	parser.add_argument(
		"--timestamp",
		default=None,
		help="SPDX creation timestamp. Defaults to SOURCE_DATE_EPOCH or the HEAD commit time.",
	)
	parser.add_argument(
		"--reference",
		default=None,
		help=(
			"Git reference recorded as Celero's provenance in downloadLocation, the package URL and "
			"the document namespace. Defaults to the HEAD commit SHA, which is right for a document "
			"built from a working tree. Pass a tag (e.g. v2.10.0) for documents that are committed "
			"to the repository or attached to a release, so they pin the release rather than a "
			"commit that moves."
		),
	)
	parser.add_argument(
		"--pinned",
		action="store_true",
		help=(
			"Produce the committed/release form of the documents: pin --reference to v<version> and "
			"--timestamp to the commit that last modified vcpkg.json. Output then depends only on "
			"the version and the resolved dependency set, so the documents under sbom/ stay "
			"byte-stable across unrelated commits. This is the form checked into the repository."
		),
	)
	parser.add_argument(
		"--allow-missing-vcpkg",
		action="store_true",
		help=(
			"Emit the source SBOM even when no vcpkg SPDX output is found. Off by default so an "
			"incomplete dependency list is never published silently."
		),
	)
	return parser.parse_args(argv)


def main(argv=None):
	args = parse_args(argv)

	project_root = Path(args.project_root).resolve()
	output_dir = Path(args.output_dir).resolve() if args.output_dir else project_root / "sbom"

	warnings = []
	version = args.version or read_project_version(project_root)
	commit = run_git(project_root, "rev-parse", "HEAD")

	if args.pinned:
		reference = args.reference or "v{}".format(version)
		if args.timestamp:
			timestamp = args.timestamp
		else:
			epoch = run_git(project_root, "log", "-1", "--format=%ct", "--", "vcpkg.json")
			if epoch is None:
				raise GeneratorError(
					"--pinned needs the commit date of vcpkg.json, which requires a git checkout. "
					"Pass --timestamp explicitly when generating outside a repository."
				)
			timestamp = datetime.fromtimestamp(int(epoch), timezone.utc).strftime(SPDX_TIME_FORMAT)
	else:
		reference = args.reference or commit or "v{}".format(version)
		timestamp = resolve_timestamp(args.timestamp, project_root)

	discovered = discover_vcpkg_spdx(project_root, args.vcpkg_installed, args.triplet)
	written = []

	if args.flavor in ("source", "both"):
		spdx_paths = [path for paths in discovered.values() for path in paths]
		if not spdx_paths and not args.allow_missing_vcpkg:
			raise GeneratorError(
				"No vcpkg SPDX documents found. Configure the project first, for example:\n"
				"    cmake -S . -B build -DCELERO_ENABLE_TESTS=ON "
				"-DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake\n"
				"or pass --vcpkg-installed, or --allow-missing-vcpkg to accept an incomplete document."
			)

		# One triplet's resolution is representative: the manifest declares no
		# platform-conditional dependencies, so prefer a stable choice.
		if discovered:
			chosen_triplet = sorted(discovered)[0]
			spdx_paths = discovered[chosen_triplet]
			if len(discovered) > 1:
				warnings.append(
					"Found vcpkg output for {} triplets; using '{}' for the source SBOM.".format(
						len(discovered), chosen_triplet
					)
				)

		document = generate_source_document(
			project_root, version, reference, commit, timestamp, spdx_paths, warnings
		)
		written.append(write_document(document, output_dir / "celero-{}-source.spdx.json".format(version)))

	if args.flavor in ("library", "both"):
		if args.triplet:
			triplets = [args.triplet]
		elif args.pinned:
			triplets = list(DEFAULT_TRIPLETS)
		else:
			triplets = sorted(discovered)

		if not triplets:
			raise GeneratorError(
				"Cannot determine a triplet for the library SBOM. Pass --triplet (e.g. x64-linux), "
				"use --pinned to emit the published set ({}), or configure the project so vcpkg "
				"output is discoverable.".format(", ".join(DEFAULT_TRIPLETS))
			)

		for triplet in triplets:
			document = generate_library_document(
				project_root, version, reference, commit, timestamp, triplet
			)
			written.append(
				write_document(document, output_dir / "celero-{}-{}.spdx.json".format(version, triplet))
			)

	for warning in warnings:
		print("warning: {}".format(warning), file=sys.stderr)

	for path in written:
		print(path)

	return 0


if __name__ == "__main__":
	try:
		sys.exit(main())
	except GeneratorError as error:
		print("error: {}".format(error), file=sys.stderr)
		sys.exit(1)
