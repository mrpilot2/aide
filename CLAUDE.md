# CLAUDE.md

Guidance for Claude Code in this repository.

## Project Overview

**aIDE** — C++20/Qt6 (min 6.5) framework library for Qt desktop apps. Provides action registry, settings management, key binding UI, logging infrastructure. Consumers embed as a CMake dependency and compose an `Application` via `ApplicationBuilder`.

## Build System

CMake 3.26+ with presets (`CMakePresets.json`) + Conan 2 (`conanfile.txt`), invoked automatically via `conan_provider.cmake`.

Preset naming: `{role}-{platform}-{linkage}-{build-type}`

| Role | What it enables |
|------|----------------|
| `client` | Plain build, no extras |
| `dev` | clang, warnings-as-errors, sanitizers (ASAN+UBSAN), coverage, clang-tidy, cppcheck |
| `ci-strict` | clang, warnings-as-errors, sanitizers |
| `ci-static` | clang, warnings-as-errors, clang-tidy, cppcheck |

```bash
# Full workflow (configure + build + test) — always use 'dev-' presets to verify changes
cmake --workflow --preset workflow-dev-unix-static-debug

# Or step by step:
cmake --preset dev-unix-static-debug
cmake --build --preset build-dev-unix-static-debug
ctest --preset test-dev-unix-static-debug

# Minimal build without tooling (faster iteration):
cmake --preset client-unix-static-debug
cmake --build --preset build-client-unix-static-debug
ctest --preset test-client-unix-static-debug
```

Build output: `build/<preset-name>/`.

### Run a single test

Catch2 3.x, binary at `tests/ut/ut.aide` inside the build dir:

```bash
./build/client-unix-static-debug/tests/ut/ut.aide "test name or pattern"
./build/client-unix-static-debug/tests/ut/ut.aide --list-tests        # list all
./build/client-unix-static-debug/tests/ut/ut.aide "[tag]"             # by tag
```

### Formatting & linting

Pre-commit hooks enforce formatting (`.clang-format`, `.clang-tidy`, `.cmake-format.yaml`):

```bash
pre-commit run --all-files          # all hooks
pre-commit run clang-format         # C++ formatting only
pre-commit run cmake-format         # CMake formatting only
```

clang-tidy/cppcheck run at build time in `dev`/`ci-static` presets, not as separate commands.

## Architecture

```
ApplicationBuilder  ←  consumer entry point
    └── Application  (lifecycle, owns logger + settings + main window)
         ├── core/   (business logic, no Qt GUI dependency)
         │    ├── UseCase classes  (ShowKeymap, ShowSettingsDialog, …)
         │    ├── ActionRegistry   (maps HierarchicalId → QAction)
         │    ├── Settings         (QSettings wrapper behind SettingsInterface)
         │    └── HierarchicalId   (dot-separated ID system for actions/settings, e.g. "aide.file.open")
         ├── gui/    (Qt widgets and dialogs)
         │    ├── MainWindow
         │    ├── settings/  (SettingsDialog + page tree model + KeymapTreeModel)
         │    └── widgets/   (SearchLineEdit, AideTreeView, AideTableView, …)
         └── logger/ (spdlog-based, implements LoggerInterface)
```

Key interfaces (`aide/include/aide/`): `LoggerInterface`, `SettingsInterface`, `ActionRegistryInterface`, `MenuContainerInterface`, `TranslatorInterface` — decouple core/gui from concrete Qt/spdlog dependencies for testability.

`demo/` is a runnable example app — reference for `ApplicationBuilder` usage.

## Conventions

- C++20 throughout; no exceptions in public API (Qt signal/slot style error handling).
- Conventional Commits (`feat:`, `fix:`, `ci:`, `chore:`, …) — enforced by pre-commit hook.
- release-please manages releases from `main`; development happens on `develop`.

## Common pitfalls

- **Magic numbers** (`dev`/`ci-static`): `readability-magic-numbers` is an error — extract all numeric literals (including e.g. `QColor(220, 235, 255)`) to named `constexpr`.
- **cppcheck virtualCallInConstructor**: calling a virtual (e.g. `reset()`) from a ctor is flagged — extract a private non-virtual helper (e.g. `syncToModel()`) called by both.
- **clang-format rewrites on commit**: if `git commit` fails this way, the hook already fixed the file — `git add` it and re-commit. Never `--no-verify`.
- **Catch2 cognitive complexity**: clang-tidy caps it (~25) — split deep `SECTION` nesting into separate `TEST_CASE`s.
- **Qt icon theme layout**: `QIcon::fromTheme()` needs `<qrc-prefix>/<theme-name>/index.theme` and `<qrc-prefix>/<theme-name>/scalable/<category>/<icon-name>.svg`, prefix matching `QIcon::setThemeSearchPaths()`.
- **SettingsPageRegistry is instance-based**, not a singleton — access via `app.settingsPageRegistry()`; no `deleteAllPages()`, lifetime is tied to `ApplicationBuilder`.

## Agent skills

- **Issue tracker**: mrpilot2/aide GitHub Issues via `gh` CLI; external PRs are not a triage surface. See `docs/agents/issue-tracker.md`.
- **Triage labels**: `needs-triage`, `needs-info`, `ready-for-agent`, `ready-for-human`, `wontfix`. See `docs/agents/triage-labels.md`.
- **Domain docs**: single-context layout (`CONTEXT.md` + `docs/adr/` at repo root). See `docs/agents/domain.md`.
