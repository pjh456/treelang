# tests/

## Responsibility
CTest-registered smoke test target proving the build graph links; no real assertions yet.

## Design
- Single executable `treelang_tests` from `test_smoke.cpp`, a plain `main()` that prints `"Hello Tree Lang Test!"`.
- No test framework, no assert/expect — success is implicit (clean run, exit 0).
- Registered with CTest via `add_test(NAME treelang_tests COMMAND treelang_tests)`.
- Gated by root option `TREELANG_ENABLE_TESTS` (default `ON`), which calls `enable_testing()` and `add_subdirectory(tests)`.

## Flow
1. `treelang_tests` links `treelang_core` privately (transitive PJH headers visible).
2. Compile definition `TREELANG_BUILD_TESTS` is set on this target only — the core library can conditionally expose test hooks.
3. `ctest` runs the binary; current body prints one line and exits.

## Integration
- Consumed by: CTest / CI.
- Depends on: `treelang_core` (and through it the `pjh_*` libraries).
- Note: does not exercise any gameplay code, because none exists in `src/` yet.
