# Repository Atlas: treelang

## Project Responsibility
Traditional dungeon-crawl MUD game written in C++20 (design phase). Player descends
randomly-connected 5x5 floor maps, fights enemies using an elemental "ritual" (术式)
skill system with attribute-counter crits, a flow-state combo chain, SAN/HP/shield
status, S.P.E.C.I.A.L. character stats, and item fusion.

**Implementation status: design complete, code NOT started.** The entire README design
document is unimplemented. The current working tree is a build scaffold that links the
four PJH third-party libraries and a smoke test that prints Hello World.

## System Entry Points
- `src/main.cpp`: trivial entry point; exercises `pjh_result`, `pjh_json`, `pjh_platform`
  APIs and prints Hello World strings. No game logic.
- `src/nothing.cpp`: intentionally empty; keeps `treelang_core` a valid CMake target.
- `tests/test_smoke.cpp`: one-line CTest target (prints text, no assertions).
- `CMakeLists.txt`: project definition; C++20; pulls in 4 `thirdparty/` submodules,
  then `src/`, then optional `tests/`.
- `README.md`: full game design spec (Chinese) — combat, flow/心流, SAN, stats, rooms,
  items, achievements.
- `Prop_Alternate.md`: candidate item/effect list for the item system (unimplemented).
- `.gitmodules`: 4 git submodules under `thirdparty/`.

## Directory Map (Aggregated)
| Directory / File | Responsibility Summary | Detailed Map |
|------------------|------------------------|--------------|
| `src/` | Entry point + core library scaffold; links the 4 PJH libs; zero game logic. | [codemap](src.md) |
| `tests/` | Single CTest smoke target; no assertions yet. | [codemap](tests.md) |
| `thirdparty/pjh_result` | Header-only C++20 port of Rust's `Result<T,E>` + `Option<T>`; three-state tagged union, monadic combinators, `?`-style `TRY`/`ASSIGN_OR_RETURN` macros. | — |
| `thirdparty/pjh_json` | SIMD-accelerated C++20 JSON parser; custom 24-byte tagged-union `Json`, zero-copy strings, PMR allocators, constexpr JSON, `dump`/`dump_jsonl`. | — |
| `thirdparty/pjh_cli` | C++20 CLI framework; compile-time option keys, subcommand tree, fuzzy matching, REPL/console mode, builder-style options. | — |
| `thirdparty/pjh_platform` | C++20 cross-platform abstraction (env vars, filesystem, file watcher, directory snapshot/diff, OS detection, encoding); all fallible APIs return `pjh::result::Result<T, ErrorCode>`. | — |

## Design vs Implementation Gap
| Spec (README) | Code |
|---|---|
| 术式 (skill/ritual) system, elemental damage, 黑闪 crit | — |
| 心流 flow state, combo chain, key-propagation, 熔断 cooldown penalty | — |
| 5x5 floor map, 5 room types, initial/resupply room | — |
| 咒力 (mana), 血量 HP, SAN (理智), 护盾 shield, 失控 state | — |
| S.P.E.C.I.A.L. 7-stat system, level-ups, attribute points (cap 20) | — |
| Item fusion, item system (消耗品/战术道具) | — |
| Achievements + permanent cross-save rewards | — |
| Multi-结局 (multiple endings) | — |

Only the build plumbing exists. The full game requires new source modules under `src/`
for: skill/ritual model + damage formula, flow-state state machine, map/floor generator,
room/NPC/event system, player status (mana/HP/SAN/shield), stat allocation, item system,
and save/achievement persistence.

## Build
```bash
git submodule update --init --recursive
cmake -S . -B build -DTREELANG_ENABLE_TESTS=ON
cmake --build build --config Release
ctest --test-dir build --output-on-failure
```
Note: `thirdparty/` submodules are **not** checked out in this working copy; the LSP
reports `pjh` and `<iostream>` as unresolvable until `git submodule update --init`
is run.

## Remotes
- `origin` — `lkjing2021345/treelang` (user fork)
- `upstream` — `pjh456/treelang` (original)
- Branches: `main`, `feature/hyh` (current).
