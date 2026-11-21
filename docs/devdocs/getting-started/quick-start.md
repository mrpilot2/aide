# Quick Start

This page walks you through building a minimal aIDE application from scratch. The complete, runnable version lives in the [`demo/`](https://github.com/mrpilot2/aide/tree/main/demo) directory.

---

## 1. Set application identity

Before constructing `aide::Application`, set the Qt application metadata. aIDE uses these values as the root keys for `QSettings`, so they must be provided before any settings are read:

```cpp
#include <aide/application.hpp>

int main(int argc, char* argv[])
{
    aide::Application::setOrganizationName("MyCompany");
    aide::Application::setApplicationName("MyApp");
    aide::Application::setApplicationDisplayName("My Application");

    const aide::Application app(argc, argv);
    // ...
}
```

!!! warning
    Constructing `aide::Application` without setting `OrganizationName` and `ApplicationName` first will cause a runtime assertion failure.

---

## 2. Access the main window

`app.mainWindow()` returns a `shared_ptr<QMainWindow>`. Set your central widget on it:

```cpp
#include <QLabel>

auto mainWindow = app.mainWindow();
mainWindow->setCentralWidget(
    new QLabel("Hello from aIDE!", mainWindow.get()));
```

The main window already has a `File` and `Help` menu. You do not need to create them.

---

## 3. Add a settings page

Subclass `aide::core::SettingsPage`, implement the four required methods, then register it:

```cpp
#include <aide/settings/settingspage.hpp>
#include <aide/settings/settingspageregistry.hpp>
#include <aide/aidesettingsprovider.hpp>
#include <aide/hierarchicalid.hpp>

aide::core::SettingsPageRegistry::addPage(
    std::make_unique<MySettingsPage>(
        aide::HierarchicalId("My App")("Preferences"),
        aide::AideSettingsProvider::unversionableSettings()));
```

The `HierarchicalId` controls where the page appears in the settings tree. See [Adding Settings Pages](../core/settings/adding-settings-pages.md) for details on implementing `SettingsPage`.

---

## 4. Extend the File menu

Use the action registry to look up an existing menu and insert an action:

```cpp
#include <aide/aideconstants.hpp>
#include <aide/menucontainerinterface.hpp>

using aide::constants::CONSTANTS;

auto actionRegistry = app.actionRegistry();
auto fileMenuContainer = actionRegistry->getMenuContainer(
    aide::HierarchicalId("Main Menu")("File"));

auto actionNew = std::make_shared<QAction>(QObject::tr("New Project..."));

if (fileMenuContainer.has_value()) {
    auto* fileMenu = fileMenuContainer.value()->menu();
    actionNew->setParent(fileMenu);

    // Insert before the Settings action
    fileMenu->insertAction(
        actionRegistry->action(CONSTANTS().FILE_SETTINGS).value(),
        actionNew.get());

    // Register so it appears in the keymap settings page
    actionRegistry->registerAction(
        actionNew,
        aide::HierarchicalId("Main Menu")("File")("New Project"));
}
```

---

## 5. Run the event loop

```cpp
return aide::Application::exec();
```

---

## Complete minimal example

```cpp
#include <QLabel>

#include <aide/aideconstants.hpp>
#include <aide/aidesettingsprovider.hpp>
#include <aide/application.hpp>
#include <aide/menucontainerinterface.hpp>
#include <aide/settings/settingspageregistry.hpp>

int main(int argc, char* argv[])
{
    aide::Application::setOrganizationName("MyCompany");
    aide::Application::setApplicationName("MyApp");
    aide::Application::setApplicationDisplayName("My Application");

    const aide::Application app(argc, argv);

    app.mainWindow()->setCentralWidget(
        new QLabel("Hello from aIDE!", app.mainWindow().get()));

    return aide::Application::exec();
}
```
