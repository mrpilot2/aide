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
| `Help` | enabled | Web-based End-User Help; only active once a Help Base URL is set (see [Help](#help)) |

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

## Help

Help opens Help Pages from the client's own web help in the browser. It is **active** only when `Feature::Help` is enabled (the default) **and** a Help Base URL is set. With the feature on but no URL, help stays inactive and that is logged once; it is not an error.

```cpp
aide::ApplicationConfig config;
config.setHelpBaseUrl(QUrl("https://example.com/help"))
      .remapHelpTopic(aide::HelpTopicId{"keymap"}, aide::HelpTopicId{"shortcuts"})
      .disableHelpTopic(aide::HelpTopicId{"notifications"});

aide::Application app(argc, argv, config);
app.showHelp(aide::HelpTopicId{"appearance"}); // opens https://example.com/help/appearance/
app.showHelpContents();                        // opens https://example.com/help/
```

A **Help Topic ID** (`aide::HelpTopicId`, header `aide/helptopicid.hpp`) is a relative path; nesting with `/` is allowed (`settings/keymap`). A leading `/`, a `..` segment, `?` or `#` make it invalid: it becomes an empty ID and a warning is logged.

A topic resolves to `<Help Base URL>/<topic-id>/`, matching mkdocs directory URLs; a missing trailing slash on the base is added.

- `remapHelpTopic(source, target)` sends a topic to another location. Remaps apply one step only (`a→b`, `b→c` sends `a` to `b`), and the last registration wins.
- `disableHelpTopic(id)` turns help off for a topic. It is checked against the declared ID, before any remap, so remapping onto a disabled ID does not disable the topic.

`Application::showHelp()`, `isHelpAvailable()` and `showHelpContents()` open URLs through the configured URL launcher (see above). They return `false`, launching nothing, when help is inactive or the topic is empty or disabled.

While help is active, Help → "Help Contents" heads the Help menu and opens the Help Base URL root, like `showHelpContents()`. It has no default shortcut; users can assign one in the Keymap. While help is inactive the action is neither created nor registered, so it is absent from both the menu and the Keymap. It has no `Feature` value of its own.

### Widgets and Context Help

Bind a Help Topic to any widget, including stock Qt ones, with the free functions in `aide/widgethelptopic.hpp`. Calling `setHelpTopic` again replaces the topic, and an empty ID clears it:

```cpp
#include <aide/widgethelptopic.hpp>

aide::setHelpTopic(workspace, aide::HelpTopicId{"workspace"});
aide::helpTopic(workspace); // HelpTopicId{"workspace"}
```

While help is active, the main window has a **Context Help** action (`HELP_CONTEXT_HELP`). Its default shortcut is `QKeySequence::HelpContents` (F1, or Cmd+? on macOS), and users can remap it in the Keymap. It is not shown in the Help menu. When triggered, it starts at the focused widget and walks up the parent chain to the first widget with a topic. A topic on a child therefore wins over its container's. The walk stops at the focused widget's window: the window's own topic still counts, but a parent window's does not. If the walk finds no topic, or the topic is disabled, Context Help opens Help Contents instead. While help is inactive the action is neither created nor registered, so the key does nothing.

### Settings pages

A settings page declares its Help Topic by overriding `SettingsPage::helpTopicId()`. The default returns an empty ID, so pages that don't override it still compile and simply have no topic:

```cpp
aide::HelpTopicId MySettingsPage::helpTopicId() const
{
    return aide::HelpTopicId{"my-settings"};
}
```

aIDE's built-in pages declare `appearance`, `keymap` and `notifications`. These IDs are **stable public API**, so renaming or removing one is a breaking change.

The settings dialog's Help button is **hidden** while help is inactive. It is **disabled** while the selected page or group node has no available topic (`isHelpAvailable()` is false), and **enabled** otherwise. Clicking it opens the selected page's topic. Its state updates on every selection change.

---

## Value semantics

`ApplicationConfig` is an ordinary copyable value with a private implementation (pImpl), so its binary layout stays stable as new toggles are added — extending it is an additive enum value plus a default-table entry, never a public layout change. Copies deep-copy their overrides, so a copy can be modified without affecting the original.
