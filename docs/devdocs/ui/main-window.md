# Main Window

aIDE provides a pre-built `QMainWindow` with a standard menu bar. You access it via `app.mainWindow()` and set your application's central widget on it.

---

## Accessing the main window

```cpp
auto mainWindow = app.mainWindow();  // shared_ptr<QMainWindow>
```

The returned `shared_ptr` is owned by `aide::Application`. Do not delete it; the application manages its lifetime.

---

## Setting the central widget

```cpp
auto* myWidget = new MyMainWidget(mainWindow.get());
mainWindow->setCentralWidget(myWidget);
```

The central widget is the primary content area. It is `nullptr` by default, so calling `setCentralWidget` is required to show any content.

---

## Pre-built menu bar

The main window's menu bar is populated during `Application` construction with:

| Menu | Items |
|---|---|
| **File** | Settings, *(separator)*, Quit |
| **Help** | About Aide, About Qt |

You can extend either menu using the action registry. See [Action Registry](../core/action-registry.md) for details on inserting and separating actions.

---

## Accessing the menu bar

The menu bar is the standard Qt `QMenuBar*`:

```cpp
mainWindow->menuBar()->addMenu(myCustomMenu);
```

After inserting menus or actions programmatically, call `mainWindow->menuBar()->update()` to repaint.

---

## Window geometry persistence

aIDE automatically saves and restores the main window's size, position, and state (docked widgets, toolbars) to the versionable settings store. This happens transparently — no code is required.
