# Changelog

## 2.11.1

### Build

- Celero now targets C++14 (#180). The library and experiments compile as C++14, and `target_compile_features` requires dependents to use at least C++14. A consumer that asks for a newer standard keeps it. The test binary stays on C++17 because GoogleTest 1.18 requires it.

### Dependencies

- Refresh the vcpkg baseline (#181, #182, #183, #184) to `277978ad2eddc3b05ecf288dcd33071968cd2f79`. Resolved ports are unchanged: gtest 1.18.0, vcpkg-cmake 2026-11-09, and vcpkg-cmake-config 2026-07-21.
