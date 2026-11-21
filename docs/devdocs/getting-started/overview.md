# Overview

aIDE is a C++20/Qt framework library that provides the structural scaffolding every Qt desktop application needs — menus, settings, keybindings, logging — so that you can build on top of a consistent, extensible foundation rather than reinventing it.

The design is inspired by the [IntelliJ Platform](https://plugins.jetbrains.com/docs/intellij/welcome.html): your application is a thin layer on top of a pre-wired platform that handles the infrastructure.

---

## Architecture

```
ApplicationBuilder  ←  internal wiring (created automatically by Application)
    └── Application  (derives from QApplication; owns all platform services)
         ├── core/
         │    ├── ActionRegistry      registers and looks up QActions by HierarchicalId
         │    ├── Settings            QSettings wrapper behind SettingsInterface
         │    └── HierarchicalId      dot-separated ID for actions and settings keys
         ├── gui/
         │    ├── MainWindow          the top-level QMainWindow with pre-built menus
         │    ├── SettingsDialog      tree-structured settings dialog
         │    └── widgets/            AideTreeView, AideTableView, SearchFilterWidget, …
         └── logger/                  spdlog-based, implements LoggerInterface
```

`aide::Application` is the single entry point. Constructing it wires everything together. You then compose your application on top by:

- Setting the central widget on the main window
- Adding settings pages to `SettingsPageRegistry`
- Registering actions with the action registry
- Extending existing menus

---

## Key abstractions

| Abstraction | Header | Purpose |
|---|---|---|
| `aide::Application` | `aide/application.hpp` | Derives from `QApplication`; owns all platform services |
| `aide::HierarchicalId` | `aide/hierarchicalid.hpp` | Dot-separated ID used to address actions, menus, and settings keys |
| `aide::ActionRegistryInterface` | `aide/actionregistryinterface.hpp` | Register and look up `QAction`s; create and extend menus |
| `aide::SettingsInterface` | `aide/settingsinterface.hpp` | `QSettings` wrapper; get/set values by `HierarchicalId` |
| `aide::AideSettingsProvider` | `aide/aidesettingsprovider.hpp` | Provides the two shared settings stores (versionable and unversionable) |
| `aide::core::SettingsPage` | `aide/settings/settingspage.hpp` | Base class for custom settings pages |
| `aide::core::SettingsPageRegistry` | `aide/settings/settingspageregistry.hpp` | Static registry; call `addPage()` to add your pages |
| `aide::LoggerInterface` | `aide/loggerinterface.hpp` | Logging abstraction backed by spdlog |
| `aide::gui::TranslatorInterface` | `aide/gui/translatorinterface.hpp` | Register additional Qt translation files |

---

## Pre-built menus and actions

aIDE creates a `File` and `Help` menu automatically. The following actions are registered and available via `CONSTANTS()` (from `aide/aideconstants.hpp`):

| Constant | Menu location | Default behaviour |
|---|---|---|
| `CONSTANTS().FILE_SETTINGS` | File → Settings | Opens the settings dialog |
| `CONSTANTS().FILE_QUIT` | File → Quit | Closes the application |
| `CONSTANTS().HELP_ABOUT_AIDE` | Help → About Aide | Opens the About aIDE dialog |
| `CONSTANTS().HELP_ABOUT_QT` | Help → About Qt | Opens the Qt About dialog |

You can insert your own actions before or after these using [`ActionRegistryInterface`](../core/action-registry.md).
