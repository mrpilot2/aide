# ApplicationConfig

`ApplicationConfig` is a consumer-facing set of feature toggles handed to `aide::Application` (or `ApplicationBuilder`) at construction time. It declares which of aIDE's built-in features the application uses.

**Header:** `aide/applicationconfig.hpp`

Every feature defaults to enabled, so a zero-configuration application behaves exactly like plain aIDE — with two exceptions (see below) that default to disabled.

---

## Passing a config at construction

```cpp
aide::ApplicationConfig config;
config.setEnabled(aide::ApplicationConfig::Feature::ShowLogInFileManagerAction, true);

const aide::Application app(argc, argv, config);
```

`setEnabled()` returns `*this`, so calls chain fluently:

```cpp
aide::ApplicationConfig config;
config.setEnabled(aide::ApplicationConfig::Feature::ReportBugAction, true)
      .setEnabled(aide::ApplicationConfig::Feature::ViewFullscreenAction, false);
```

The active config is readable back via `app.config()`.

---

## Available features

| `Feature` | Default | Description |
|---|---|---|
| `ViewFullscreenAction` | enabled | Checkable View → Full Screen action bound to F11 |
| `ShowLogInFileManagerAction` | **disabled** | Help → "Show Log in \<File Manager\>" action |
| `ReportBugAction` | **disabled** | Help → "Report Bug in aIDE" action |

`ShowLogInFileManagerAction` and `ReportBugAction` default to disabled because they expose concepts (the log file location, aIDE's own issue tracker) that end users of most aIDE-based applications have no reason to know about. Enable them for developer-facing tools.

```cpp
[[nodiscard]] bool isEnabled(Feature feature) const;
```

`isEnabled()` returns the consumer's override if one was set via `setEnabled()`, otherwise the feature's built-in default.

---

## Overriding the URL launcher

`ReportBugAction` (and any other feature that opens a URL) uses an OS-backed launcher by default. Substitute your own:

```cpp
class MyUrlLauncher : public aide::UrlLauncherInterface
{
public:
    bool openUrl(const std::string& url) const override
    {
        // custom handling, e.g. routing through an embedded browser
        return true;
    }
};

aide::ApplicationConfig config;
config.setUrlLauncher(std::make_shared<MyUrlLauncher>());
```

`urlLauncher()` returns `nullptr` if no override was set, in which case callers fall back to the default OS-backed launcher.

---

## Value semantics

`ApplicationConfig` is an ordinary copyable value with a private implementation (pImpl), so its binary layout stays stable as new toggles are added — extending it is an additive enum value plus a default-table entry, never a public layout change. Copies deep-copy their overrides, so a copy can be modified without affecting the original.
