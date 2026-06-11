# Application Lifecycle

`aide::Application` is the single entry point for every aIDE-based program. It derives from `QApplication` and is usable in the same way — the key difference is that constructing it also wires the entire platform infrastructure.

**Header:** `aide/application.hpp`

---

## Pre-construction requirements

Two Qt application properties **must** be set before constructing `aide::Application`, because aIDE reads `QSettings` during construction and Qt uses these values as the settings root:

```cpp
aide::Application::setOrganizationName("MyCompany");  // required
aide::Application::setApplicationName("MyApp");        // required
aide::Application::setApplicationDisplayName("My Application"); // optional, for title bars
```

!!! warning
    Constructing `aide::Application` without setting organization and application name causes a runtime assertion failure.

---

## Construction

```cpp
const aide::Application app(argc, argv);
```

During construction, `Application` internally creates and wires:

- **Logger** — spdlog-backed; log files are written to the platform's standard app data location
- **Settings stores** — two `SettingsInterface` instances (versionable and unversionable) backed by `QSettings`
- **Main window** — a `QMainWindow` with pre-built `File` and `Help` menus
- **Action registry** — populated with the built-in actions (`FILE_SETTINGS`, `FILE_QUIT`, `HELP_ABOUT_AIDE`, `HELP_ABOUT_QT`)
- **Settings dialog** — including the built-in keymap settings page

---

## Accessing platform services

All platform services are accessible via the `Application` instance:

```cpp
auto mainWindow    = app.mainWindow();       // shared_ptr<QMainWindow>
auto actionReg     = app.actionRegistry();   // shared_ptr<ActionRegistryInterface>
auto settings      = app.settingsProvider(); // shared_ptr<AideSettingsProvider>
auto translator    = app.translator();       // shared_ptr<TranslatorInterface>
```

The logger is accessed statically (because it is also used before the app object exists):

```cpp
auto logger = aide::Application::logger();              // default logger
auto named  = aide::Application::logger("my_module");   // named logger
```

---

## Disabling console logging

By default, log output goes to both a log file and the console. To suppress console output (useful in release builds):

```cpp
aide::Application::disableLoggingToConsole();
```

Call this before construction so it takes effect from the first log message.

---

## Event loop

```cpp
return aide::Application::exec();
```

This is the standard `QApplication::exec()` — it blocks until the application quits.

---

## Shutdown

Cleanup happens automatically when `Application` goes out of scope. There is no explicit shutdown API. Settings are saved, the main window is closed, and the logger flushes.
