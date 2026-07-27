# Celero SBOM Generation

Celero publishes SPDX 2.3 Software Bills of Materials. They are committed to the
repository under [`sbom/`](../../sbom), produced by
[`generate_sbom.py`](generate_sbom.py), and attached to each GitHub Release —
with a build provenance attestation — by the
[`SBOM` workflow](../../.github/workflows/sbom.yml).

Because the documents are committed, CI's job is to prove they are still true:
every push and pull request regenerates them and fails if the result differs
from what is checked in.

## Two documents, two questions

Celero ships one SBOM per release *plus* one per supported platform, because
"what does Celero contain?" and "what will I be linking against?" have different
answers.

| Document | Answers | Contents |
| --- | --- | --- |
| `celero-<version>-source.spdx.json` | What is in the source distribution? | celero, the vcpkg-resolved test and build dependencies, the vendored third-party headers under `experiments/`, and the GitHub Actions that build it |
| `celero-<version>-<triplet>.spdx.json` | What does a consumer link against? | celero and the operating-system libraries it requires — nothing else |

The library document is deliberately small. Celero has **no third-party runtime
dependencies**. `gtest` is the only declared vcpkg dependency and it is consumed
solely by the `celero-test` target behind `CELERO_ENABLE_TESTS` (`OFF` by
default), so it never enters the installed artifact and never appears in a
library SBOM. Anyone auditing Celero's dependency exposure should read the
library document; anyone auditing the repository should read the source
document.

## Regenerating the committed documents

Configure once so vcpkg resolves the dependency set, then run the generator:

```sh
cmake -S . -B build -DCELERO_ENABLE_TESTS=ON \
  -DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake
python tools/sbom/generate_sbom.py --pinned
```

That single command rewrites all four documents in `sbom/`, and is exactly what
CI runs when it checks them. Commit the result.

`--pinned` is what makes the committed documents stable: it pins Celero's
provenance to `v<version>` rather than the `HEAD` commit, and the SPDX `created`
timestamp to the commit that last modified `vcpkg.json`. The output therefore
depends only on the version and the resolved dependency set — it does not change
when unrelated commits land, so the freshness check flags real changes only.

For ad-hoc generation, `--flavor`, `--triplet`, `--reference`, `--timestamp` and
`--output-dir` are all available; a library document needs nothing but a triplet
name, while a source document needs the vcpkg configure above. Run `--help` for
the full list.

## Design notes

**Dependency facts are read, not restated.** vcpkg writes an SPDX document for
every port it installs, at
`<vcpkg_installed>/<triplet>/share/<port>/vcpkg.spdx.json`. The generator reads
those files, so resolved versions, upstream URLs, source SHA-512 checksums and
ABI hashes come from the tool that actually performed the resolution rather than
from a hand-maintained list that can drift.

**Vendored source is declared by hand, because no scanner can infer it.** Two
third-party headers were copied into `experiments/` and neither is discoverable
by dependency tooling:

- `experiments/ExperimentCostSharedPtr/osg_ref_ptr.h` — from OpenSceneGraph,
  © 1998-2006 Robert Osfield, under the **OpenSceneGraph Public License**, not
  Celero's Apache-2.0. The OSGPL has no SPDX short identifier, so it is emitted
  as `LicenseRef-OSGPL` with its full text in `hasExtractedLicensingInfos`.
- `experiments/ExperimentParticles/Particles.h` — derived from
  [fenbf/benchmarkLibsTest](https://github.com/fenbf/benchmarkLibsTest). This
  file carries **no license header** and the upstream license has not been
  established, so it is recorded as `NOASSERTION` rather than assumed to be
  Apache-2.0. This is an open question, not a settled fact.

Both compile only when `CELERO_ENABLE_EXPERIMENTS=ON` and neither is linked into
the installed library, which is why they appear in the source document only.
Their content is SHA-256 hashed at generation time, so the SBOM pins the exact
copy in the tree.

**The generator fails rather than under-reports.** An SBOM that is quietly
missing a component is worse than no SBOM, so the script exits non-zero when:

- no vcpkg SPDX output can be found for a source document (override with
  `--allow-missing-vcpkg` if you genuinely want an incomplete document); or
- a header listed in `VENDORED_COMPONENTS` is no longer at its recorded path,
  which catches the case where vendored code is moved or removed and the SBOM
  silently stops describing reality.

A vcpkg port that is not classified in `PORT_SCOPE` is reported on stderr and
conservatively treated as a build dependency, so a newly added *runtime*
dependency cannot be silently misfiled as build-only. When you add a dependency
to `vcpkg.json`, add it to `PORT_SCOPE` as well.

**Output is reproducible.** Without `--pinned`, the SPDX `created` timestamp
defaults to `SOURCE_DATE_EPOCH` and falls back to the `HEAD` commit time, so
regenerating from the same commit is byte-identical. With `--pinned` the output
carries no `HEAD`-dependent field at all and is byte-identical even when
generated outside a git checkout, which is what makes the committed copies
diffable in CI.

## Validation

CI validates every document against the reference SPDX toolkit and fails the
build on any finding:

```sh
python -m pip install "spdx-tools==0.8.3"
pyspdxtools -i sbom/celero-2.10.0-source.spdx.json
```

## Maintenance

Rerun `python tools/sbom/generate_sbom.py --pinned` and commit whenever the
inventory changes — CI will tell you if you forget. In addition, the generator
itself needs editing when:

- a dependency is added to `vcpkg.json` → add it to `PORT_SCOPE`, otherwise it
  is reported on stderr and filed as build-only;
- third-party source is vendored into the tree → add it to
  `VENDORED_COMPONENTS`;
- the set of system libraries in `CMakeLists.txt` changes → update
  `SYSTEM_LIBRARIES`;
- a platform is added to or removed from the release matrix → update
  `DEFAULT_TRIPLETS`.

The project version is read from `vcpkg.json` and needs no separate bump, though
bumping it does change every document's filename, so regenerate on release.
GitHub Actions are discovered by parsing `.github/workflows/*.yml`, so pipeline
changes are picked up automatically.
