# Help API

aIDE's help system opens **Help Pages** from your application's own web help in the user's browser. You write and host the help site (mkdocs-material is the supported setup); aIDE gives you:

- the Help menu's **Help Contents** action, a **Context Help** action (F1 by default) and a **Help** button in the Settings dialog;
- C++ calls to bind **Help Topics** to settings pages and widgets, and to remap or disable them;
- End-User Help pages for its own Settings dialog pages, to include in your site (see [Building the Help Site](help-site.md));
- `Application::declaredHelpTopics()`, so a test can check that every topic has a page (see [Help Coverage Test](coverage-test.md)).

aIDE never checks at runtime that a page exists, and never adds a version to a URL. Versioning and fallback for unknown topics are your docs server's job (see [Versioned Help](versioning.md)).

---

## Turning help on

Set a **Help Base URL**, the root of your help site, on the [`ApplicationConfig`](../fundamentals/application-config.md):

```cpp
#include <aide/application.hpp>
#include <aide/applicationconfig.hpp>

aide::ApplicationConfig config;
config.setHelpBaseUrl(QUrl("https://example.com/help/"));

aide::Application app(argc, argv, config);
```

Help is **active** only when both hold:

- `ApplicationConfig::Feature::Help` is enabled. It is enabled by default.
- A Help Base URL is set with `setHelpBaseUrl()`.

With the feature on but no URL, help stays inactive and this is logged once; it is not an error. To switch the whole help system off, even with a URL set, disable the feature:

```cpp
config.setEnabled(aide::ApplicationConfig::Feature::Help, false);
```

While help is inactive, aIDE shows no help UI at all: the Help Contents and Context Help actions are neither created nor registered (so they are absent from the menu and the Keymap), and the Settings dialog's Help button is hidden.

`helpBaseUrl()` returns the URL that was set, or an empty `QUrl`.

---

## Help Topic IDs

A **Help Topic ID** (`aide::HelpTopicId`, header `aide/helptopicid.hpp`) names one Help Page. It is a relative path under the Help Base URL:

- Nesting with `/` is allowed: `keymap`, `settings/keymap`.
- A leading `/`, a `..` segment, `?` or `#` make it invalid. Invalid input becomes an **empty ID** (no topic) and a warning is logged. No exception is thrown.
- `value()` returns the ID as a `QString`, `isEmpty()` tells whether it is empty, and IDs compare with `==`, `!=` and `<`.

A topic resolves to `<Help Base URL>/<topic-id>/`, which matches mkdocs directory URLs (`use_directory_urls: true`, the default): the page `docs/settings/keymap.md` is served at `settings/keymap/`. A missing trailing slash on the base URL is added; otherwise the base URL is used exactly as given.

```cpp
config.setHelpBaseUrl(QUrl("https://example.com/help"));
// HelpTopicId{"appearance"}       -> https://example.com/help/appearance/
// HelpTopicId{"settings/keymap"}  -> https://example.com/help/settings/keymap/
```

---

## Binding topics

### Settings pages

A settings page declares its topic by overriding `SettingsPage::helpTopicId()`. The default returns an empty ID: the page has no topic.

```cpp
aide::HelpTopicId MySettingsPage::helpTopicId() const
{
    return aide::HelpTopicId{"my-settings"};
}
```

The Settings dialog's **Help** button opens the selected page's topic. It is hidden while help is inactive, disabled while the selected page or group node has no available topic, and enabled otherwise; its state updates on every selection change.

Context Help also works inside the Settings dialog: with focus anywhere in it, Context Help opens the selected page's topic, unless a widget inside the page has its own topic. On a page without a topic, Context Help opens Help Contents while the Help button stays disabled. The dialog uses the current Keymap keys of Context Help each time it opens, so a remapped key works in the next dialog opened.

### Widgets

Bind a topic to any widget, including stock Qt ones, with the free functions in `aide/widgethelptopic.hpp`:

```cpp
#include <aide/widgethelptopic.hpp>

aide::setHelpTopic(workspace, aide::HelpTopicId{"workspace"});
aide::helpTopic(workspace); // HelpTopicId{"workspace"}
```

Calling `setHelpTopic()` again replaces the topic, an empty ID clears it, and a null widget is ignored. `helpTopic()` returns the widget's own topic only; it does not consult parents.

**Context Help** (`HELP_CONTEXT_HELP`, default shortcuts: every platform binding of `QKeySequence::HelpContents`, such as F1, or Cmd+? on macOS; remappable in the Keymap; not shown in the Help menu) starts at the focused widget and walks up the parent chain to the first widget with a topic. A topic on a child therefore wins over its container's, and a topic on a container covers all its children. The walk stops at the focused widget's window: the window's own topic still counts, a parent window's does not. If the walk finds no topic, or the topic is disabled, Context Help opens Help Contents.

### Declaring widget topics

Widget topics are only known once the widget exists. Declare every topic you pass to `setHelpTopic()` up front with `ApplicationConfig::declareHelpTopic()`, so that [`declaredHelpTopics()`](#listing-every-topic) sees it even when the widget is created lazily:

```cpp
config.declareHelpTopic(aide::HelpTopicId{"workspace"})
      .declareHelpTopic(aide::HelpTopicId{"editor/tabs"});
```

Settings pages need no declaration: their `helpTopicId()` is collected from the settings page registry. Declaring an empty ID or the same ID twice has no further effect. `ApplicationConfig::declaredWidgetHelpTopics()` returns the declared widget topics as given, sorted, before any disable or remap.

The first time Context Help resolves to a topic that was never declared (neither a settings page's topic nor passed to `declareHelpTopic()`), it logs a warning naming the topic, once per topic. Context Help still opens it.

---

## Remapping and disabling topics

Use these to fit topics (aIDE's built-in ones in particular) into your site's structure, or to suppress them:

```cpp
config.remapHelpTopic(aide::HelpTopicId{"keymap"}, aide::HelpTopicId{"settings/shortcuts"})
      .disableHelpTopic(aide::HelpTopicId{"notifications"});
```

- `remapHelpTopic(source, target)` sends `source` to the location of `target`. Remaps apply **one step only**: with `a→b` and `b→c`, `a` goes to `b`. The last registration for a `source` wins. A remap onto an empty `target` is ignored with a logged warning, and `source` keeps its declared ID: a remap only redirects, to turn a topic off use `disableHelpTopic`. `remappedHelpTopic(id)` returns the target, or `id` if it was not remapped.
- `disableHelpTopic(id)` turns help off for a topic. It is checked against the **declared** ID, before any remap, so remapping onto a disabled ID does not disable the source. `isHelpTopicDisabled(id)` tells whether it was disabled.

A disabled topic behaves like no topic: the Help button is disabled, `showHelp()` returns `false` and Context Help falls back to Help Contents.

---

## Opening help from code

`Application` (header `aide/application.hpp`) exposes:

| Call | What it does |
|---|---|
| `bool showHelp(const HelpTopicId& id)` | Opens the topic's page, after any remap. Returns `false`, launching nothing, when `isHelpAvailable(id)` is false; otherwise whether the launcher succeeded. |
| `bool isHelpAvailable(const HelpTopicId& id) const` | `true` when help is active and `id` is non-empty and not disabled. |
| `bool showHelpContents()` | Opens the Help Base URL itself. Returns `false` when help is inactive. |
| `std::vector<HelpTopicId> declaredHelpTopics() const` | Every topic the application can open, see below. |

```cpp
app.showHelp(aide::HelpTopicId{"appearance"}); // https://example.com/help/appearance/
app.showHelpContents();                        // https://example.com/help/
```

URLs open through the configured URL launcher (see [Overriding the URL launcher](../fundamentals/application-config.md#overriding-the-url-launcher)); without one, the operating system's default browser is used.

The Help menu's **Help Contents** action (`HELP_CONTENTS`) heads the Help menu and opens the Help Base URL, like `showHelpContents()`. It has no default shortcut; users can assign one in the Keymap.

### Listing every topic

`declaredHelpTopics()` lists every Help Topic the application can open, so a test can check that each one has a page. It:

1. collects the `helpTopicId()` of every registered settings page plus the topics passed to `declareHelpTopic()`;
2. drops disabled IDs (checked against the declared ID);
3. applies remaps, one step;
4. returns the result **sorted and de-duplicated**.

It ignores whether help is active: the list is the same with no Help Base URL or with `Feature::Help` off. Call it after your settings pages are registered.

```cpp
// declareHelpTopic("workspace"), declareHelpTopic("editor/tabs"), no other setup:
app.declaredHelpTopics(); // appearance, editor/tabs, keymap, notifications, workspace
```

With `disableHelpTopic(keymap)`, `keymap` is left out. With `remapHelpTopic(keymap, settings/keys)`, `settings/keys` takes its place. Two topics remapped onto one target appear once.

---

## Built-in Help Topics

aIDE's built-in Settings dialog pages declare these topics:

| Help Topic ID | Settings page |
|---|---|
| `appearance` | Appearance |
| `keymap` | Keymap |
| `notifications` | Notifications |

The **IDs are stable public API**: renaming or removing one is a breaking change, announced as such in the changelog, because your stubs and published page paths depend on them. The **content** of aIDE's pages for them is not stable API: it changes with aIDE releases and arrives with your next aIDE upgrade and docs build. To keep an old URL working after you move a page, use `remapHelpTopic()` or a redirect in your docs site.

[Building the Help Site](help-site.md) shows how to include aIDE's pages for these topics. The [demo application](../demo/walkthrough.md#end-user-help) uses all of the above.
