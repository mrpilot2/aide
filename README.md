 # aIDE

> A JetBrains IntelliJ-inspired application platform for C++/Qt desktop applications.

aIDE eliminates the boilerplate of building a Qt desktop application — giving you a pre-wired action registry, settings dialog, configurable keybindings, and structured logging out of the box, so you can focus on your application's unique functionality.

Think of it as the [IntelliJ Platform](https://www.jetbrains.com/opensource/idea/), but for C++20 and Qt.

[![CI](https://github.com/mrpilot2/aide/actions/workflows/ci_main.yml/badge.svg)](https://github.com/mrpilot2/aide/actions/workflows/ci_main.yml)
[![codecov](https://codecov.io/gh/mrpilot2/aide/branch/develop/graph/badge.svg)](https://codecov.io/gh/mrpilot2/aide)
[![CodeQL](https://github.com/mrpilot2/aide/actions/workflows/codeql.yml/badge.svg)](https://github.com/mrpilot2/aide/actions/workflows/codeql.yml)

[![All Contributors](https://img.shields.io/github/all-contributors/mrpilot2/aide?color=ee8449&style=flat-square)](Contributors.md)

---

## Table of Contents

- [Features](#features)
- [Roadmap](#roadmap)
- [Requirements](#requirements)
- [Integration](#integration)
- [Usage](#usage)
- [Building from Source](#building-from-source)
- [License](#license)
- [Contributing](#contributing)
- [Contributors](#contributors-)

---

## Features

- **Action Registry** — register `QAction`s under a `HierarchicalId` (e.g. `"Main Menu"/"File"/"New"`) and look them up or bind them to menus anywhere in your application
- **Settings Dialog** — a tree-structured settings UI; add your own pages with a single `SettingsPageRegistry::addPage()` call
- **Configurable Keybindings** — per-action key sequences stored in `QSettings`, editable through the built-in keybinding settings page
- **Structured Logging** — [spdlog](https://github.com/gabime/spdlog)-based logging behind a `LoggerInterface`, so your components stay decoupled from the logging backend
- **Translation Support** — `TranslatorInterface` integrates Qt's `QTranslator` with an easy hook for loading additional translation files
- **Qt6** — built and shipped against Qt 6.5+
- **C++20** — modern, interface-driven design with no exceptions in public API

---

## Roadmap

The following features are planned for future releases:

- Notification system
- Background task management
- Version control integration
- Appearance settings (fonts, themes, color schemes)
- Additional settings infrastructure

---

## Requirements

| Dependency | Minimum version |
|------------|----------------|
| CMake      | 3.25            |
| C++ compiler | C++20 (GCC 10+, Clang 12+, MSVC 2019 16.11+) |
| Qt         | Qt 6.5+ |
| Conan      | 2.x *(recommended for dependency management)* |

**Platforms:** Linux, macOS, Windows.

aIDE's own dependencies (`fmt`, `spdlog`, `Catch2`) are managed automatically via [Conan 2](https://conan.io/) using the bundled `conan_provider.cmake`.

---

## Integration

The recommended way to consume aIDE is via CMake's `FetchContent`.

### Step 1 — Set up Conan (recommended)

aIDE requires `fmt` and `spdlog`. The easiest way to provide them is to use the same `conan_provider.cmake` hook that aIDE uses internally:

```cmake
# In your top-level CMakeLists.txt, before project()
set(CMAKE_PROJECT_TOP_LEVEL_INCLUDES
    "${CMAKE_SOURCE_DIR}/cmake/conan_provider.cmake"
)
```

Copy [`conan_provider.cmake`](cmake/conan_provider.cmake) into your project's `cmake/` directory, then create a `conanfile.txt` that includes at minimum:

```ini
[requires]
fmt/12.1.0
spdlog/1.17.0

[generators]
CMakeDeps
```

Alternatively, provide `fmt` and `spdlog` through any other CMake-compatible package manager.

### Step 2 — Fetch and link aIDE

```cmake
include(FetchContent)

FetchContent_Declare(
    aide
    GIT_REPOSITORY https://github.com/mrpilot2/aide.git
    GIT_TAG        v0.3.0
)
FetchContent_MakeAvailable(aide)

target_link_libraries(my_app PRIVATE Aide::AideApplication)
```

---

## Usage

A minimal aIDE application:

```cpp
#include <aide/application.hpp>
#include <aide/settings/settingspageregistry.hpp>
#include <aide/menucontainerinterface.hpp>
#include <aide/aideconstants.hpp>

#include <QLabel>

int main(int argc, char* argv[])
{
    // Must be set before constructing Application
    aide::Application::setOrganizationName("MyCompany");
    aide::Application::setApplicationName("MyApp");
    aide::Application::setApplicationDisplayName("My Application");

    const aide::Application app(argc, argv);

    // Access the main window provided by aIDE
    auto mainWindow = app.mainWindow();
    mainWindow->setCentralWidget(new QLabel("Hello from aIDE!"));

    // Register a custom settings page
    aide::core::SettingsPageRegistry::addPage(
        std::make_unique<MySettingsPage>(
            aide::HierarchicalId("My App")("Preferences"),
            aide::AideSettingsProvider::unversionableSettings()));

    // Extend an existing menu
    auto actionRegistry = app.actionRegistry();
    auto fileMenu = actionRegistry->getMenuContainer(
        aide::HierarchicalId("Main Menu")("File"));

    if (fileMenu.has_value()) {
        auto* action = new QAction(QObject::tr("New Project..."));
        fileMenu.value()->menu()->addAction(action);
        actionRegistry->registerAction(
            std::shared_ptr<QAction>(action),
            aide::HierarchicalId("Main Menu")("File")("New Project"));
    }

    return aide::Application::exec();
}
```

A fully working example is available in the [`demo/`](demo/) directory.

### Key concepts

| Concept | Description |
|---------|-------------|
| `aide::Application` | Derives from `QApplication`; owns the main window, logger, settings, and action registry |
| `aide::HierarchicalId` | Dot-separated ID used to address actions and settings (e.g. `"Main Menu"/"File"/"New"`) |
| `aide::core::SettingsPageRegistry` | Static registry; call `addPage()` to add your own pages to the settings dialog |
| `ActionRegistryInterface` | Look up menus and actions by `HierarchicalId`; register new actions for keybinding support |
| `LoggerInterface` | Inject into your components for structured, spdlog-backed logging |

---

## Building from Source

aIDE uses CMake presets for all build configurations.

### Prerequisites

- CMake 3.25+
- Conan 2.x (`pip install conan`)
- Clang or GCC (recommended for the `dev` preset)
- Qt 6.5+

### Quick start (developer build)

```bash
# Configure, build, and test in one step
cmake --workflow --preset workflow-dev-unix-static-debug
```

### Step by step

```bash
cmake --preset dev-unix-static-debug
cmake --build --preset build-dev-unix-static-debug
ctest --preset test-dev-unix-static-debug
```

### Preset roles

| Role | Description |
|------|-------------|
| `client` | Plain build, no extra tooling |
| `dev` | Clang, warnings-as-errors, ASan + UBSan, coverage, clang-tidy, cppcheck |
| `ci-strict` | Clang, warnings-as-errors, sanitizers |
| `ci-static` | Clang, warnings-as-errors, clang-tidy, cppcheck |

Presets follow the naming pattern `{role}-{platform}-{linkage}-{build_type}`, e.g. `dev-unix-static-debug` or `client-windows-shared`.

### Running a single test

Tests use [Catch2 3.x](https://github.com/catchorg/Catch2):

```bash
./build/client-unix-static-debug/tests/ut/ut.aide "test name or pattern"
./build/client-unix-static-debug/tests/ut/ut.aide --list-tests
./build/client-unix-static-debug/tests/ut/ut.aide "[tag]"
```

### Build options

The following CMake options are available (most are set automatically by presets):

| Option                            | Default                     | Description                       |
|-----------------------------------|-----------------------------|-----------------------------------|
| `AIDE_ENABLE_DEMO`                | `ON`                        | Build the demo application        |
| `AIDE_ENABLE_COVERAGE`            | `OFF`                       | Enable coverage reporting         |
| `AIDE_ENABLE_HARDENING`           | `ON`                        | Enable compiler hardening flags   |
| `AIDE_ENABLE_SANITIZER_ADDRESS`   | platform-dependent          | Enable AddressSanitizer           |
| `AIDE_ENABLE_SANITIZER_UNDEFINED` | platform-dependent          | Enable UndefinedBehaviorSanitizer |
| `AIDE_ENABLE_CLANG_TIDY`          | `OFF` (client) / `ON` (dev) | Enable clang-tidy                 |
| `AIDE_ENABLE_CPPCHECK`            | `OFF` (client) / `ON` (dev) | Enable cppcheck                   |
| `AIDE_ENABLE_CACHE`               | `OFF` (client) / `ON` (dev) | Enable ccache                     |
| `AIDE_ENABLE_IPO`                 | `OFF`                       | Enable link-time optimization     |
| `AIDE_WARNINGS_AS_ERRORS`         | `OFF` (client) / `ON` (dev) | Treat warnings as errors          |

---

## License

aIDE is licensed under the **[GNU General Public License v3.0](LICENSE)**.

> **Important:** GPLv3 is a copyleft license. If you embed aIDE in your application and distribute it, your application must also be released under a GPL-compatible open-source license. Embedding aIDE in a closed-source or proprietary application is **not permitted** under this license.

aIDE includes third-party open-source libraries under their own licenses. See [3rd-party-licenses.md](3rd-party-licenses.md) for details.

---

## Contributing

See the [contributing docs](CONTRIBUTING.md) for more information.

---

## Contributors ✨

This project follows the [all-contributors](https://github.com/all-contributors/all-contributors) specification. Contributions of any kind are welcome.

See the full list of [contributors](Contributors.md).
