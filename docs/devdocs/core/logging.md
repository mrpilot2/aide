# Logging

aIDE provides a structured, spdlog-backed logging system behind a `LoggerInterface`. Components receive a `LoggerPtr` via injection, keeping them decoupled from the logging backend.

**Headers:**  
- `aide/loggerinterface.hpp` — the interface  
- `aide/logger/loggerfactory.hpp` — for creating named loggers outside of `Application`  
- `aide/logger/log_helper_macros.hpp` — macro-based logging (avoids `shared_ptr` overhead in hot paths)

---

## Accessing the default logger

The default logger is always available via `aide::Application::logger()`:

```cpp
auto log = aide::Application::logger();
log->info("Application started");
log->debug("Loading config from {}", configPath);
```

---

## Named loggers

Named loggers write to a separate log sink, making it easier to filter output by module:

```cpp
auto log = aide::Application::logger("my_module");
log->info("Module initialised");
```

Multiple calls with the same name return the same logger instance.

To create a named logger outside of an `Application` (e.g. in a library component):

```cpp
#include <aide/logger/loggerfactory.hpp>

auto log = aide::core::LoggerFactory::createLogger("my_module");
```

---

## Log levels

```cpp
log->trace("Very detailed, high-frequency trace: value={}", x);
log->debug("Debug information useful during development");
log->info("Normal operational message");
log->warn("Something unexpected but recoverable: {}", detail);
log->error("An error occurred: {}", errorMessage);
log->critical("Unrecoverable failure: {}", fatalDetail);
```

All methods accept `std::format`-style format strings.

### Setting the log level

```cpp
log->setLevel(aide::LogLevel::Warn);  // suppress Trace, Debug, Info
```

`LogLevel` values: `Trace`, `Debug`, `Info`, `Warn`, `Error`, `Critical`, `Off`.

---

## Disabling console output

By default, log messages go to both the console and a log file. To suppress the console sink:

```cpp
aide::Application::disableLoggingToConsole();
```

Call this **before** constructing `aide::Application`.

---

## Macro-based logging

For hot paths where constructing a `shared_ptr<LoggerInterface>` per call is undesirable, use the logging macros from `aide/logger/log_helper_macros.hpp`. They call into spdlog directly:

```cpp
#include <aide/logger/log_helper_macros.hpp>

// Default logger
AIDE_LOG_TRACE("trace message: {}", value);
AIDE_LOG_DEBUG("debug: {}", value);
AIDE_LOG_INFO("info: {}", value);
AIDE_LOG_WARN("warn: {}", value);
AIDE_LOG_ERROR("error: {}", value);
AIDE_LOG_CRITICAL("critical: {}", value);

// Named logger
AIDE_CUSTOM_LOG_INFO("my_module", "info from module: {}", value);
AIDE_CUSTOM_LOG_ERROR("my_module", "error in module: {}", value);
```

If the named logger does not exist, the macros fall back to the default logger and emit a warning about the missing name.

---

## Injecting the logger into components

The recommended pattern is to accept a `LoggerPtr` in the constructor, allowing test doubles and different loggers per component:

```cpp
#include <aide/loggerinterface.hpp>

class MyComponent
{
public:
    explicit MyComponent(aide::LoggerPtr logger)
        : m_log(std::move(logger))
    {}

    void doWork()
    {
        m_log->info("Starting work");
        // ...
    }

private:
    aide::LoggerPtr m_log;
};

// At the call site:
MyComponent comp(aide::Application::logger("my_component"));
```

---

## Log file location

Log files are written to the platform's standard application data directory:

| Platform | Path |
|---|---|
| Linux | `~/.local/share/<OrgName>/<AppName>/` |
| macOS | `~/Library/Application Support/<OrgName>/<AppName>/` |
| Windows | `%APPDATA%\<OrgName>\<AppName>\` |

The file name is `<AppName>.log`. Old logs are rotated automatically.
