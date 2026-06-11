# Action Registry

The action registry is the central catalogue of all `QAction`s in your application. It maps each action to a `HierarchicalId`, stores its default and user-overridden key sequences, and organises menus hierarchically.

**Header:** `aide/actionregistryinterface.hpp`  
**Access:** `app.actionRegistry()` → `shared_ptr<ActionRegistryInterface>`

---

## Registering actions

There are four `registerAction()` overloads, giving you control over description and default key sequences:

```cpp
auto actionRegistry = app.actionRegistry();
auto myAction = std::make_shared<QAction>(QObject::tr("New Project..."));

// Minimal — no description, no default shortcut
actionRegistry->registerAction(
    myAction,
    aide::HierarchicalId("Main Menu")("File")("New Project"));

// With description (shown in the keymap settings page)
actionRegistry->registerAction(
    myAction,
    aide::HierarchicalId("Main Menu")("File")("New Project"),
    "Create a new project");

// With default key sequence
actionRegistry->registerAction(
    myAction,
    aide::HierarchicalId("Main Menu")("File")("New Project"),
    {QKeySequence(Qt::CTRL | Qt::Key_N)});

// With both description and default key sequences
actionRegistry->registerAction(
    myAction,
    aide::HierarchicalId("Main Menu")("File")("New Project"),
    "Create a new project",
    {QKeySequence(Qt::CTRL | Qt::Key_N)});
```

!!! note
    The `QAction` is stored as a `weak_ptr`. You are responsible for keeping the `shared_ptr` alive for as long as the action should be active. Attaching the action as a child of a menu or widget (via `setParent`) is a common pattern.

---

## Looking up actions

`action()` returns a `std::optional<QAction*>`:

```cpp
#include <aide/aideconstants.hpp>
using aide::constants::CONSTANTS;

auto opt = actionRegistry->action(CONSTANTS().FILE_SETTINGS);
if (opt.has_value()) {
    opt.value()->setEnabled(false);
}
```

`actions()` returns a reference to the full `std::map<HierarchicalId, Action>`:

```cpp
for (const auto& [id, entry] : actionRegistry->actions()) {
    // entry.action       — weak_ptr<QAction>
    // entry.description  — std::string
    // entry.defaultKeySequences
    // entry.keySequences (user-overridden)
    // entry.getActiveKeySequences() — returns user sequences if set, else defaults
}
```

---

## Creating and extending menus

### Creating a menu

`createMenu()` creates a new `QMenu` wrapped in a `MenuContainerInterface` and registers it under the given `HierarchicalId`:

```cpp
aide::MenuContainerInterface* viewMenu =
    actionRegistry->createMenu(aide::HierarchicalId("Main Menu")("View"));

viewMenu->menu()->addAction(myViewAction.get());
app.mainWindow()->menuBar()->addMenu(viewMenu->menu());
```

An optional `QWidget* parent` overload lets you set the menu's parent widget.

### Extending an existing menu

`getMenuContainer()` returns a `std::optional<MenuContainerInterface*>` for a menu that aIDE (or your own code) already created:

```cpp
auto fileMenuOpt = actionRegistry->getMenuContainer(
    aide::HierarchicalId("Main Menu")("File"));

if (fileMenuOpt.has_value()) {
    QMenu* fileMenu = fileMenuOpt.value()->menu();
    fileMenu->insertAction(
        actionRegistry->action(CONSTANTS().FILE_SETTINGS).value(),
        myNewAction.get());
    fileMenu->insertSeparator(
        actionRegistry->action(CONSTANTS().FILE_SETTINGS).value());
}
```

---

## Pre-registered menus and actions

aIDE pre-registers the following menus and actions. Use `CONSTANTS()` from `aide/aideconstants.hpp` to reference them:

| ID | Type | Description |
|---|---|---|
| `CONSTANTS().MENU_FILE` | Menu | `File` top-level menu |
| `CONSTANTS().MENU_HELP` | Menu | `Help` top-level menu |
| `CONSTANTS().FILE_SETTINGS` | Action | Opens the settings dialog |
| `CONSTANTS().FILE_QUIT` | Action | Quits the application |
| `CONSTANTS().HELP_ABOUT_AIDE` | Action | Opens the About aIDE dialog |
| `CONSTANTS().HELP_ABOUT_QT` | Action | Opens the About Qt dialog |

---

## Modifying shortcuts programmatically

User-customised shortcuts are stored in `QSettings` and reloaded automatically. You can also modify them programmatically:

```cpp
actionRegistry->modifyShortcutsForAction(
    aide::HierarchicalId("Main Menu")("File")("New Project"),
    {QKeySequence(Qt::CTRL | Qt::Key_N), QKeySequence(Qt::CTRL | Qt::Key_T)});
```

This persists the shortcut to the user's settings, overriding the default. Passing an empty list restores the default.
