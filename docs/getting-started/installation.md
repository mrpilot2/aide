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

## Requirements

| Dependency | Minimum version |
|---|---|
| CMake | 3.25 |
| C++ compiler | C++20 — GCC 10+, Clang 12+, or MSVC 2019 16.11+ |
| Qt | 5.15 LTS **or** 6.5+ |
| Conan | 2.x *(if using the Conan integration)* |

Qt 6 is preferred when available. aIDE builds and tests against both Qt 5.15 and Qt 6.5+ in CI.

---

## CMake options

The following options can be passed to CMake when consuming aIDE via `FetchContent`:

| Option | Default | Description |
|---|---|---|
| `aide_ENABLE_DEMO` | `ON` | Build the demo application |
| `aide_ENABLE_HARDENING` | `ON` | Enable compiler hardening flags |
| `aide_ENABLE_IPO` | `OFF` | Enable link-time optimization |

Set them with `-Daide_ENABLE_DEMO=OFF` on the CMake command line or via `FetchContent_Declare`'s `CMAKE_ARGS`.
