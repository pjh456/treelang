# src/

## Responsibility
Application entry point and core library scaffold for the Tree Lang dungeon MUD game. Currently a smoke-test harness for the four `pjh_*` third-party libraries; no game logic is implemented yet.

## Design
- **Build layout**: `treelang_core` (static-ish library, currently empty body via `nothing.cpp`) + `treelang` (executable). C++20, no extensions.
- **Dependency layering**: `treelang` (executable) links `treelang_core` privately; `treelang_core` links `pjh_result`, `pjh_json`, `pjh_cli`, `pjh_platform` publicly, so the executable transitively sees the PJH headers.
- **Placeholder pattern**: `nothing.cpp` is intentionally empty to keep `treelang_core` a valid CMake target before real sources exist.
- **Library usage pattern**: `main.cpp` demonstrates the value-result idiom (`pjh::result::Result<T, std::nullopt_t>::Ok(...)` + `.unwrap()`), JSON value wrapper construction/serialization (`pjh::json::Json(std::string)` + `.as_string()`), and platform event-kind enumeration (`pjh::platform::FileEventKind::Created`).

## Flow
1. `main()` constructs a `Result<std::string, std::nullopt_t>` with `Ok("Hello World from PJH_RESULT!")` and prints it via `.unwrap()`.
2. Constructs `pjh::json::Json("Hello World from PJH_JSON!")` and prints `.as_string()`.
3. References `pjh::platform::FileEventKind::Created` (discarded enum value — compiles, no runtime effect).
4. Prints `"Hello World!"` and exits 0.

## Integration
- Consumed by: nothing externally; `treelang` is the final binary. `tests/` links `treelang_core` and defines `TREELANG_BUILD_TESTS`.
- Depends on: `pjh_result`, `pjh_json`, `pjh_cli`, `pjh_platform` (git submodules under `thirdparty/`; not checked out in this working copy).
- Not yet wired to: any gameplay system from the design spec (`README.md`: flow/heartflow combat, SAN/HP/shield, S.P.E.C.I.A.L. stats, 5x5 map rooms, item fusion) — all unimplemented.
