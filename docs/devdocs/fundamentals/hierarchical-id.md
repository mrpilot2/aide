# HierarchicalId

`aide::HierarchicalId` is the central addressing scheme in aIDE. It is a dot-separated string identifier that represents a path through a tree — used as the key for actions, menus, and settings values.

**Header:** `aide/hierarchicalid.hpp`

---

## Constructing a HierarchicalId

A `HierarchicalId` is constructed from a root-level `const char*`, then extended level by level using `operator()` or `addLevel()`:

```cpp
// Single-level ID
aide::HierarchicalId id("Main Menu");
// name() → "Main Menu"

// Multi-level via operator()
aide::HierarchicalId fileNew = aide::HierarchicalId("Main Menu")("File")("New");
// name() → "Main Menu.File.New"

// Equivalent with addLevel()
aide::HierarchicalId fileNew2 =
    aide::HierarchicalId("Main Menu").addLevel("File").addLevel("New");
```

### Chaining modes

`operator()` and `addLevel()` have two overloads each:

- **Non-const (in-place):** mutates the existing object and returns a reference.
- **Const (building from an existing id):** returns a new object, leaving the original unchanged.

```cpp
aide::HierarchicalId base("Main Menu");

// Building new IDs from a shared base
aide::HierarchicalId fileMenu = base("File");   // const overload — base unchanged
aide::HierarchicalId helpMenu = base("Help");   // base is still "Main Menu"
```

This is how `aide::constants::CONSTANTS()` defines all its IDs: each constant is built from a shared base.

---

## Getting the string representation

```cpp
aide::HierarchicalId id = aide::HierarchicalId("Main Menu")("File")("New");
std::string s = id.name();  // "Main Menu.File.New"
```

---

## Comparisons

`HierarchicalId` supports `==`, `!=`, `<`, and `>`, making it usable as a `std::map` key:

```cpp
aide::HierarchicalId a("Foo")("Bar");
aide::HierarchicalId b("Foo")("Bar");
assert(a == b);

std::map<aide::HierarchicalId, int> myMap;
myMap[a] = 42;
```

---

## Iterating over levels

`begin()` and `end()` iterate over the raw `const char*` levels:

```cpp
aide::HierarchicalId id("A")("B")("C");
for (auto it = id.begin(); it != id.end(); ++it) {
    // *it is "A", then "B", then "C"
}
```

---

## Usage in aIDE

| Context | Example |
|---|---|
| Registering a menu | `actionRegistry->createMenu(HierarchicalId("Main Menu")("File"))` |
| Registering an action | `actionRegistry->registerAction(action, HierarchicalId("Main Menu")("File")("Save"))` |
| Looking up an action | `actionRegistry->action(HierarchicalId("Main Menu")("File")("Save"))` |
| Settings key | `settings->setValue(HierarchicalId("MyApp")("fontSize"), 12)` |
| Settings page group | `SettingsPage(HierarchicalId("My App")("Editor")("Fonts"))` |

---

## Pre-defined IDs

aIDE ships with a set of pre-defined IDs in `aide/aideconstants.hpp`. Access them through the `CONSTANTS()` function:

```cpp
#include <aide/aideconstants.hpp>

using aide::constants::CONSTANTS;

auto fileSettingsAction = actionRegistry->action(CONSTANTS().FILE_SETTINGS);
auto fileQuitAction     = actionRegistry->action(CONSTANTS().FILE_QUIT);
```

| Constant | Value |
|---|---|
| `CONSTANTS().MAIN_MENU` | `"Main Menu"` |
| `CONSTANTS().MENU_FILE` | `"Main Menu.File"` |
| `CONSTANTS().MENU_HELP` | `"Main Menu.Help"` |
| `CONSTANTS().FILE_SETTINGS` | `"Main Menu.File.Settings"` |
| `CONSTANTS().FILE_QUIT` | `"Main Menu.File.Quit"` |
| `CONSTANTS().HELP_ABOUT_AIDE` | `"Main Menu.Help.About Aide"` |
| `CONSTANTS().HELP_ABOUT_QT` | `"Main Menu.Help.About Qt"` |
