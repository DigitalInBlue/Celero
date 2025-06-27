# Security Policy for Celero

This document outlines the security practices, reporting procedures, and recommendations specific to Celero’s C++ codebase.

## Supported Versions

Celero maintains security support for the two most recent minor releases of the `master` branch. Releases outside this window may not receive timely security fixes.

| Branch     | Status           |
| ---------- | ---------------- |
| `master`   | Supported        |
| `develop`  | Development only |
| Prior tags | Limited support  |

## Reporting a Vulnerability

If you discover a security vulnerability in Celero, please:

1. **Email**: send details to the maintainer.
2. **Subject**: prefix with `SECURITY REPORT:`.
3. **Include**:

   * Affected Celero version or commit SHA.
   * Description of the issue.
   * Minimal reproducible example or proof-of-concept.
   * Suggested mitigations or patches (if known).

Expect acknowledgement within **48 hours** and a fix timeline based on severity.

## Security Response Process

1. **Acknowledge** within 48 hours.
2. **Assess** impact and severity.
3. **Develop** a fix in a dedicated branch (naming: `security/<short-description>`).
4. **Review & Test**: include regression and fuzz tests.
5. **Merge** fix into `master` and backport to supported releases.
6. **Release**: draft an advisory and update the release notes.

## C++-Specific Best Practices

* **Use Sanitizers**: enable ASan, UBSan, and MSan in CI builds to catch memory errors.
* **Static Analysis**: run `clang-tidy` with security-focused checks (e.g. `cppcoreguidelines-pro-type-union-access`).
* **Fuzz Testing**: integrate libFuzzer or AFL targets for public APIs.
* **Dependency Audits**: regularly audit third-party libraries (e.g. Google Test, fmt).
* **Safe Patterns**: prefer `std::array`/`std::vector` over raw arrays; use smart pointers (`unique_ptr`, `shared_ptr`).
* **Bounds Checking**: avoid out-of-bounds access; use `at()` when indexing if safety is critical.

## Secure Development Guidelines

* **Code Reviews**: include at least one security-aware reviewer for changes to public APIs or memory management code.
* **CI Integration**: plan to add security scans (e.g. CodeQL) via GitHub Actions.
* **Secrets Management**: do **not** commit API keys or credentials; use environment variables or CI secrets.

## Acknowledgements

This policy draws on best practices from OWASP C++ Security Cheat Sheet and the GitHub security guidelines.
