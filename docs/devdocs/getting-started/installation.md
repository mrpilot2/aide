# Installation

The recommended way to consume aIDE is via CMake's `FetchContent`. aIDE requires `fmt` and `spdlog` as dependencies; the easiest way to provide them is via Conan 2.

---

## Step 1 — Set up Conan (recommended)

Copy the [`conan_provider.cmake`](https://github.com/mrpilot2/aide/blob/main/cmake/conan_provider.cmake) file from the aIDE repository into your project's `cmake/` directory, then reference it in your top-level `CMakeLists.txt` **before the `project()` call**:

```cmake
set(CMAKE_PROJECT_TOP_LEVEL_INCLUDES
    "${CMAKE_SOURCE_DIR}/cmake/conan_provider.cmake"
)
```

Create a `conanfile.txt` at your project root that includes at minimum:

```ini
[requires]
fmt/12.1.0
spdlog/1.17.0

[generators]
CMakeDeps
```

!!! tip
    If you already use another package manager (vcpkg, CPM, system packages), you can provide `fmt` and `spdlog` through it instead of Conan.

---

## Step 2 — Fetch and link aIDE

Add the following to your `CMakeLists.txt`:

```cmake
include(FetchContent)

FetchContent_Declare(
    aide
    GIT_REPOSITORY https://github.com/mrpilot2/aide.git
    GIT_TAG        v0.3.0          # replace with the desired release tag
)
FetchContent_MakeAvailable(aide)

target_link_libraries(my_app PRIVATE Aide::AideApplication)
```

That is the only target you need. `Aide::AideApplication` pulls in all transitive dependencies.

---

## Step 3 — Include headers

aIDE headers are namespaced under `aide/`. A minimal include set for a typical consumer:

```cpp
#include <aide/application.hpp>
#include <aide/actionregistryinterface.hpp>
#include <aide/hierarchicalid.hpp>
#include <aide/settings/settingspage.hpp>
#include <aide/settings/settingspageregistry.hpp>
#include <aide/aidesettingsprovider.hpp>
#include <aide/menucontainerinterface.hpp>
#include <aide/aideconstants.hpp>
```

---

## End-User Help pages

aIDE ships End-User Help pages for its built-in Help Topics (`appearance.md`,
`keymap.md`, `notifications.md`), pinned to the linked aIDE version. Include
them in your own mkdocs-material help site through `pymdownx.snippets` stubs.

The CMake variable **`AIDE_HELP_DIR`** holds the absolute path of the
directory with these pages, whichever way you consume aIDE:

| Consumption | Where `AIDE_HELP_DIR` comes from | Points to |
|---|---|---|
| `FetchContent` / `add_subdirectory` | cache entry set while aIDE is configured; visible from every directory after `FetchContent_MakeAvailable(aide)` | `<aide source>/aide/help` |
| `find_package(AideQt6)` | set by the installed package config | `<prefix>/share/aide/help` |

The installed pages belong to the `Help` install component. Install just the
pages with:

```bash
cmake --install <build-dir> --component Help --prefix <prefix>
```

Pass the directory to your docs build, for example as the snippets
`base_path`:

```cmake
add_custom_target(
    help-site
    COMMAND ${CMAKE_COMMAND} -E env "AIDE_HELP_DIR=${AIDE_HELP_DIR}"
            mkdocs build --strict
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/help-site
)
```

```yaml
# help-site/mkdocs.yml
markdown_extensions:
  - pymdownx.snippets:
      base_path:
        - !ENV AIDE_HELP_DIR
      check_paths: true
```

A stub page such as `help-site/docs/keymap.md` then contains only
`--8<-- "keymap.md"`.

---

## Requirements

| Dependency | Minimum version |
|---|---|
| CMake | 3.25 |
| C++ compiler | C++20 — GCC 10+, Clang 12+, or MSVC 2019 16.11+ |
| Qt | 6.5+ |
| Conan | 2.x *(if using the Conan integration)* |

aIDE builds and tests against Qt 6.5+ in CI.

---

## CMake options

The following options can be passed to CMake when consuming aIDE via `FetchContent`:

| Option | Default | Description |
|---|---|---|
| `AIDE_ENABLE_DEMO` | `ON` | Build the demo application |
| `AIDE_ENABLE_HARDENING` | `ON` | Enable compiler hardening flags |
| `AIDE_ENABLE_IPO` | `OFF` | Enable link-time optimization |

Set them with `-DAIDE_ENABLE_DEMO=OFF` on the CMake command line or via `FetchContent_Declare`'s `CMAKE_ARGS`.
