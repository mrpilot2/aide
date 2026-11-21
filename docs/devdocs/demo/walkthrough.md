# Demo Application Walkthrough

The [`demo/`](https://github.com/mrpilot2/aide/tree/main/demo) directory contains a fully working application that exercises every major aIDE feature. This page walks through `demo/src/main.cpp` section by section.

---

## Setting up application identity

```cpp
aide::Application::setOrganizationName("aide_company");
aide::Application::setApplicationName("Fancy_Aide_Demo");
aide::Application::setApplicationDisplayName("Fancy Aide Demo");
```

These three static calls must happen before constructing `aide::Application`. Qt uses them as the root keys for `QSettings`. See [Application Lifecycle](../fundamentals/application-lifecycle.md) for details.

---

## Constructing the application

```cpp
const aide::Application app(argc, argv);
```

This single line creates the main window, sets up the logger, initialises the two settings stores, populates the `File` and `Help` menus, and wires the settings dialog with its built-in Keymap page.

---

## Adding a translation file

```cpp
app.translator()->addAdditionalTranslationFilePath(
    QDir(":/demo_translations"), QString("demo"));
```

The demo embeds `demo_en.qm` and `demo_de.qm` in a Qt resource. aIDE loads the file matching the system locale. See [Translations](../ui/translations.md).

---

## Setting the central widget

```cpp
auto mainWindow = app.mainWindow();
auto* description = new QLabel(
    QApplication::translate("", "This text uses the translation system ..."),
    mainWindow.get());
mainWindow->setCentralWidget(description);
```

`app.mainWindow()` returns a `shared_ptr<QMainWindow>`. The demo sets a `QLabel` as the central widget to show how the translation system works.

---

## Registering settings pages

```cpp
aide::core::SettingsPageRegistry::addPage(
    std::make_unique<DemoSettingsPage>(
        aide::HierarchicalId("Demo Page 1")("Demo Subpage 1")("Demo Subpage 1.1"),
        aide::AideSettingsProvider::unversionableSettings()));

aide::core::SettingsPageRegistry::addPage(
    std::make_unique<DemoSettingsPage>(
        aide::HierarchicalId("Demo Page 1")("Demo Subpage 1")("Demo Subpage 1.2"),
        aide::AideSettingsProvider::unversionableSettings()));
```

Two settings pages are added under the same parent node (`Demo Page 1 > Demo Subpage 1`). Both share the same `DemoSettingsPage` class but have different `HierarchicalId`s, so they appear as separate leaves in the settings tree.

`AideSettingsProvider::unversionableSettings()` provides the settings store that persists across application versions. See [SettingsInterface](../core/settings/settings-interface.md).

---

## Extending the File menu

```cpp
auto actionRegistry = app.actionRegistry();
auto menuFileContainer = actionRegistry->getMenuContainer(
    aide::HierarchicalId("Main Menu")("File"));

auto actionNewProject =
    std::make_shared<QAction>(QApplication::tr("New project ..."));
```

`getMenuContainer()` looks up the pre-registered `File` menu by its `HierarchicalId`. The return type is `std::optional<MenuContainerInterface*>`.

```cpp
if (menuFileContainer.has_value()) {
    auto* menuFile = menuFileContainer.value()->menu();
    actionNewProject->setParent(menuFile);

    QObject::connect(
        actionNewProject.get(), &QAction::triggered, [mainWindow]() {
            QMessageBox::information(
                mainWindow.get(), QApplication::tr("Menu Extension"),
                QApplication::tr("This shows the menu extension capabilities."));
        });

    menuFile->insertAction(
        actionRegistry->action(CONSTANTS().FILE_SETTINGS).value(),
        actionNewProject.get());
    menuFile->insertSeparator(
        actionRegistry->action(CONSTANTS().FILE_SETTINGS).value());
    mainWindow->menuBar()->update();
```

The demo inserts `New project ...` before the `Settings` action and adds a separator between them. `actionRegistry->action(CONSTANTS().FILE_SETTINGS)` looks up the pre-registered Settings action by its well-known constant.

```cpp
    actionRegistry->registerAction(
        actionNewProject,
        aide::HierarchicalId("Main Menu")("File")("New"));
}
```

After inserting the action into the menu, it is registered with the action registry so that it appears in the Keymap settings page.

---

## Starting the event loop

```cpp
return aide::Application::exec();
```

Standard Qt event loop entry. The application runs until the user closes the window or selects File → Quit.

---

## DemoSettingsPage

`DemoSettingsPage` (in `demo/src/demosettingspage.hpp/.cpp`) demonstrates a full settings page implementation:

- Constructs a UI from a `.ui` file (`demosettingspage.ui`)
- Loads a `QTableWidget` and periodically inserts rows via a `QTimer`
- Implements `isModified()` by tracking whether the widget has been touched
- Implements `reset()` to reload values from the settings store
- Implements `apply()` to persist values to the settings store

It is the recommended reference for implementing your own settings pages. See [Adding Settings Pages](../core/settings/adding-settings-pages.md).
