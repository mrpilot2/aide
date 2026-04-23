# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**aIDE** is a C++20/Qt5+Qt6 framework library for building Qt-based desktop applications. It provides reusable components including action registry, settings management, key binding UI, and logging infrastructure. Consumers embed aIDE as a CMake dependency and compose an `Application` via `ApplicationBuilder`.

## Build System

Uses CMake 3.25+ with presets (defined in `CMakePresets.json`) and Conan 2 for dependency management (`conanfile.txt`). Conan is invoked automatically via `conan_provider.cmake` as a `CMAKE_PROJECT_TOP_LEVEL_INCLUDES` hook.

### Preset naming convention

`{role}-{platform}-{linkage}-{build-type}`

| Role | What it enables |
|------|----------------|
| `client` | Plain build, no extras |
| `dev` | clang, warnings-as-errors, sanitizers (ASAN+UBSAN), coverage, clang-tidy, cppcheck |
| `ci-strict` | clang, warnings-as-errors, sanitizers |
| `ci-static` | clang, warnings-as-errors, clang-tidy, cppcheck |

### Configure, build, test

```bash
# Full workflow (configure + build + test)
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

Always use 'dev-' presets to verify the changes and test the code.

Build output lands in `build/<preset-name>/`.

### Run a single test

Tests use **Catch2 3.x**. The test binary is `tests/ut/ut.aide` inside the build dir:

```bash
./build/client-unix-static-debug/tests/ut/ut.aide "test name or pattern"
./build/client-unix-static-debug/tests/ut/ut.aide --list-tests        # list all
./build/client-unix-static-debug/tests/ut/ut.aide "[tag]"             # by tag
```

### Code formatting & linting

Pre-commit hooks enforce formatting. To run manually:

```bash
pre-commit run --all-files          # all hooks (clang-format, cmake-format, etc.)
pre-commit run clang-format         # C++ formatting only
pre-commit run cmake-format         # CMake formatting only
```

Configuration: `.clang-format`, `.clang-tidy`, `.cmake-format.yaml`.

Clang-tidy and cppcheck run as part of `dev` and `ci-static` presets at build time (not as separate commands).

## Architecture

### Layering

```
ApplicationBuilder  ←  consumer entry point
    └── Application  (lifecycle, owns logger + settings + main window)
         ├── core/   (business logic, no Qt GUI dependency)
         │    ├── UseCase classes  (ShowKeymap, ShowSettingsDialog, …)
         │    ├── ActionRegistry   (maps HierarchicalId → QAction)
         │    ├── Settings         (QSettings wrapper behind SettingsInterface)
         │    └── HierarchicalId   (dot-separated ID system for actions/settings)
         ├── gui/    (Qt widgets and dialogs)
         │    ├── MainWindow
         │    ├── settings/  (SettingsDialog + page tree model)
         │    └── widgets/   (SearchLineEdit, AideTreeView, AideTableView, …)
         └── logger/ (spdlog-based, implements LoggerInterface)
```

### Key abstractions (all in `aide/include/aide/`)

- `LoggerInterface` — injected into all components; avoids direct spdlog dependency in consumer code
- `SettingsInterface` — wraps QSettings; allows test doubles
- `ActionRegistryInterface` — decouples action dispatch from Qt menus
- `MenuContainerInterface` / `TranslatorInterface` — further GUI isolation

### Settings & keymap

`core/settings/` manages both application preferences and key bindings. The keymap subsystem stores per-action key sequences in `SettingsInterface` and exposes them through `KeymapTreeModel` (a `QAbstractItemModel`) displayed in the settings dialog.

### HierarchicalId

Central to action and settings lookup — a dot-separated string (e.g. `"aide.file.open"`) with a tree structure backed by `TreeModel`. Used as the key in `ActionRegistry`.

### Build information

`aide/src/buildinformation/` generates a header at configure time with version, build date, and compiler info via CMake's `configure_file`.

## Conventions

- C++20 throughout; no exceptions in public API (Qt signal/slot style error handling).
- Commit messages follow **Conventional Commits** (`feat:`, `fix:`, `ci:`, `chore:`, etc.) — enforced by pre-commit hook.
- Releases are managed by **release-please** acting on the `main` branch; development happens on `develop`.
- The `demo/` directory contains a runnable example application consuming the library — useful reference for how `ApplicationBuilder` is used.
