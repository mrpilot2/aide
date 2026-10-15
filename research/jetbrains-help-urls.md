# How JetBrains IDEs build web help URLs

Research for [#185](https://github.com/mrpilot2/aide/issues/185) (map
[#184](https://github.com/mrpilot2/aide/issues/184)). Gathered 2026-10-15.

Sources are the IntelliJ Platform source code (`JetBrains/intellij-community`,
master at `b3e72adc10372a9b4c8ec17ca9598fe8b419b868`), the IntelliJ Platform SDK
docs, and live HTTP probes of `www.jetbrains.com/help/...` made with `curl` on the
date above. Source links below are pinned to that commit (`IC` is shorthand for
`https://github.com/JetBrains/intellij-community/blob/b3e72adc10372a9b4c8ec17ca9598fe8b419b868`).

## TL;DR

```
https://www.jetbrains.com[/<locale>]/help/<product>/<major.minor>/?<topicId>&keymap=<Keymap Name>[&<tracking params>]
```

- The IDE never knows page file names. It sends an **opaque topic ID as a bare
  query key** (`?preferences.keymap`) to a **versioned base URL**. The **server**
  maps the ID to a page and answers with a `302` to that page.
- Unknown topic ID: `302` to the docs start page (`getting-started.html`). The
  ID stays in the query. You never get a 404.
- Unknown or archived version: `302` to the unversioned "latest" docs
  (`/help/idea/?<topicId>`), which still resolves the topic.
- Plugins do not touch the base URL. They register a `WebHelpProvider` (extension
  point `com.intellij.webHelpProvider`). The platform sends it every topic ID that
  starts with `<pluginId>.`, and the provider returns a full URL of its own
  choosing. The old JavaHelp `helpSet` mechanism is gone from the current code path.

## 1. Where a help request comes from

| Trigger | API | Source |
|---|---|---|
| Help button in a dialog | `DialogWrapper.getHelpId()`, then `doHelpAction()`, then `HelpManager.getInstance().invokeHelp(helpId)` | `IC/platform/platform-api/src/com/intellij/openapi/ui/DialogWrapper.java` (approx. l. 1748-1764) |
| Settings page | `Configurable.getHelpTopic()` ("the topic in the help file which is shown when help for the configurable is requested") | `IC/platform/ide-core/src/com/intellij/openapi/options/Configurable.java` (l. 156-164) |
| F1 / Help action | `ContextHelpAction` reads `PlatformCoreDataKeys.HELP_ID` from the focused component's data context, or uses a fixed ID passed to its constructor, then calls `invokeHelp` | `IC/platform/platform-api/src/com/intellij/ide/actions/ContextHelpAction.java` |

In the main menu, `ContextHelpAction` is **disabled when no help ID is
available**. It does not fall back to a generic page there.

## 2. URL construction (`HelpManagerImpl.getHelpUrl`)

`IC/platform/platform-impl/src/com/intellij/help/impl/HelpManagerImpl.java`:

1. `id = id ?: "top"`. A null ID means the root topic.
2. **Plugin providers first**: for each `WebHelpProvider` in EP
   `com.intellij.webHelpProvider`, if `id.startsWith(provider.getHelpTopicPrefix())`
   and `provider.getHelpPageUrl(id) != null`, that URL is used as it is. It gets
   no keymap or tracking parameters.
3. On macOS, the native Help Book (`MacHelpUtil`) is tried if it applies.
4. Otherwise `ExternalProductResourceUrls.getInstance().getHelpPageUrl()(id)`.
   If the product sets no help URL, the result is `null` and no help is shown.
5. The parameter `keymap=<presentable name of active keymap>` is added. For a
   user-modified keymap, the name of the **predefined parent** is used.
   `KeymapHelpIdPresenterImpl` uses `keymap.presentableName`, for example
   `Default`, `macOS`, `Eclipse` (see IDEA-276358). The help web app keeps
   `keymap` as a known query parameter, so shortcuts on the page match the
   user's keymap.
6. `IdeUrlTrackingParametersProvider.augmentUrl(...)` adds tracking parameters.

The default URL function is in
`IC/platform/platform-impl/src/com/intellij/platform/ide/impl/customization/BaseJetBrainsExternalProductResourceUrls.kt`:

```kotlin
/** Returns base URL of context help pages.
 *  The current IDE version number and ID of the requested topic are added to it
 *  to get the actual URL: [baseWebHelpUrl]`/<version>/?<topicId>`. */
abstract val baseWebHelpUrl: Url?

override val helpPageUrl: ((topicId: String) -> Url)?
  get() = baseWebHelpUrl?.let { baseUrl ->
    { topicId ->
      baseUrl.resolve("${ApplicationInfo.getInstance().shortVersion}/")
             .addParameters(mapOf(topicId to ""))
    }
  }
```

- **Product segment**: each product sets `baseWebHelpUrl`:
  - `IntelliJIdeaExternalResourceUrls`: `baseWebSiteUrl.resolve("help/idea/")`
    (`IC/idea/customization/base/src/IntelliJIdeaExternalResourceUrls.kt`)
  - `PyCharmExternalResourceUrls`: `baseWebSiteUrl.resolve("help/pycharm/")`
    (`IC/python/ide-common/src/customization/PyCharmExternalResourceUrls.kt`)
- **Site / locale segment**: `baseWebSiteUrl` is
  `https://www.jetbrains.com` (or `.com.cn` for the China region), plus
  `/<lang-region>` (for example `/ja-jp`) when the UI locale is not the
  region's default.
- **Version segment**: `ApplicationInfo.shortVersion`, i.e. `major.minor`
  (`2025.2`). Bug-fix versions and build numbers are not included.
- **Topic**: the topic ID becomes a query **key with an empty value**, so the
  URL is `.../2025.2/?preferences.keymap`. It is not a path segment and not
  `?topic=...`.

Offline variant: the bundled "built-in help" plugin
(`IC/plugins/built-in-help/src/com/jetbrains/builtInHelp/BuiltInHelpManager.kt`)
replaces `HelpManager`. When online it uses the same `helpPageUrl`. When offline
it opens `http://127.0.0.1:<builtin-port>[/zh-cn]/help/?<topicId>&keymap=...`,
which is the same `?<topicId>` contract served by a local server. The ID is
URL-encoded first.

## 3. Topic ID format

- These are free-form, dot-separated, stable strings chosen by developers, for
  example `preferences.keymap`, `top`, and (for plugins) `<pluginId>.<something>`.
- The IDs are a **contract between the IDE and the docs site**, not page names.
  The same ID resolves to **different pages in different products**:
  - `https://www.jetbrains.com/help/idea/2025.2/?preferences.keymap` → `302`
    `/help/idea/2025.2/configuring-keyboard-and-mouse-shortcuts.html?preferences.keymap`
  - `https://www.jetbrains.com/help/pycharm/2025.2/?preferences.keymap` → `302`
    `/help/pycharm/2025.2/settings-keymap.html?preferences.keymap`

  So docs authors can rename or restructure pages without changing the IDE.

## 4. Server-side resolution and fallbacks (observed)

The mapping is done **server-side** with an HTTP `302`. The original query is
kept on the target URL.

| Request | Response |
|---|---|
| `/help/idea/2025.2/?preferences.keymap` | 302 → `/help/idea/2025.2/configuring-keyboard-and-mouse-shortcuts.html?preferences.keymap` |
| `/help/idea/2025.2/?no.such.topic.xyz` | 302 → `/help/idea/2025.2/getting-started.html?no.such.topic.xyz` (start page) |
| `/help/idea/2025.2/?top` | 302 → `/help/idea/2025.2/getting-started.html?top` |
| `/help/idea/2023.1/?preferences.keymap` | 302 → versioned page (2023.1 docs still hosted) |
| `/help/idea/2019.1/?preferences.keymap`, `/2016.1/`, `/2030.1/` | 302 → `/help/idea/?preferences.keymap` (unversioned latest), which then resolves the topic |
| `/help/idea/?preferences.keymap` | 302 → `/help/idea/configuring-keyboard-and-mouse-shortcuts.html?preferences.keymap` |
| `/ja-jp/help/idea/2025.2/?preferences.keymap` | 302 → `/help/idea/2025.2/?preferences.keymap` (no localized docs, falls back to English) |
| `/help/idea/2025.2/no-such-page.html` | **404** (only direct page links can 404) |

The fallbacks are layered: unknown version → latest docs, unknown topic → start
page, missing locale → English. Every fallback is on the server, and the IDE
does no checking of its own. The `BaseJetBrainsExternalProductResourceUrls`
KDoc says this outright for locales: "When localized pages are not present on
the website, it takes care of redirecting to their English equivalents, so the
IDE doesn't need to worry about that".

The current docs are built with Writerside. The page loads
`resources.jetbrains.com/writerside/apidoc/<ver>/app.js`, which keeps
`keymap` as a known query parameter.

## 5. How plugins contribute help

`IC/platform/platform-api/src/com/intellij/openapi/help/WebHelpProvider.java`:

```java
public abstract class WebHelpProvider implements PluginAware {
  /** URL to open for helpTopicId; called only when it starts with getHelpTopicPrefix(). */
  public abstract @Nullable String getHelpPageUrl(@NotNull String helpTopicId);

  @Override public final void setPluginDescriptor(@NotNull PluginDescriptor d) {
    myHelpTopicPrefix = d.getPluginId().getIdString() + '.';
  }
  public @NotNull String getHelpTopicPrefix() { return myHelpTopicPrefix; }
}
```

Registration in `plugin.xml`:

```xml
<extensions defaultExtensionNs="com.intellij">
  <webHelpProvider implementation="com.example.MyWebHelpProvider"/>
</extensions>
```

- The **namespace is enforced by the framework**. The prefix is
  `<plugin id>.`, taken from the plugin descriptor, and plugins cannot override
  it (`setPluginDescriptor` is `final`). A plugin with ID `com.example.foo` uses
  topic IDs such as `com.example.foo.settings`, returned from its
  `getHelpId()` / `getHelpTopic()` / `HELP_ID`.
- The provider gets the **full ID including the prefix** and returns **any absolute URL**,
  for example its own docs site, a GitHub wiki page, or a versioned path it
  builds itself. It must do its own version and locale handling. The SDK docs
  say: "To localize the help page URL, retrieve the current locale and language
  tag from the dynamic bundle"
  ([IDE Infrastructure → Context Help](https://plugins.jetbrains.com/docs/intellij/ide-infrastructure.html)).
- If the provider returns `null`, resolution continues to the product's own help
  (point 2.3 / 2.4 above). This is a built-in fallback.
- Providers are only asked about IDs with their prefix. A plugin cannot take
  over platform topic IDs, and it gets no keymap or tracking parameters
  automatically.
- Open-source implementations can be browsed in the
  [IntelliJ Platform Explorer](https://plugins.jetbrains.com/intellij-platform-explorer?extensions=com.intellij.webHelpProvider).
- The old `<helpset file=... path=...>` (JavaHelp `.hs` bundles) is not used by
  `HelpManagerImpl` any more. Web help through `WebHelpProvider` is the only
  plugin hook.

## 6. Implications for aIDE (Help Topic / Help Base URL)

1. **Help Topic = opaque, stable, dot-separated string ID**, attached to a
   dialog, settings page, or widget (the equivalents of `getHelpId()`,
   `getHelpTopic()` and `HELP_ID`). It fits `HierarchicalId`, for example
   `aide.settings.keymap`. A null topic should disable the context-help action,
   not open a random page.
2. **Help Base URL is set per application** (one value, like `baseWebHelpUrl`),
   and the framework adds the version and topic:
   `<base>/<major.minor>/?<topicId>`. Use a query key, not a path, so the docs
   site can map IDs to pages freely. Allow "no base URL" to mean that help is
   disabled.
3. **Fallback belongs on the docs server, not in the app.** The app should not
   probe URLs. aIDE docs should document the server contract: unknown topic →
   start page, unknown version → latest. A static docs host (e.g. mkdocs,
   which aIDE already uses) needs a small redirect page or `index.html` script
   that reads `location.search`, looks it up in a generated `topic → page` map,
   and redirects (with a start-page fallback).
4. **Extension or plugin help through prefix-routed providers**: a registry of
   `{prefix, (topicId) → optional URL}` that is checked before the
   application's base URL. The ID-namespace prefix should come from the owner's
   ID, not be chosen freely. A provider that returns nothing falls through to
   the application default.
5. **Optional context parameters** (in JetBrains: `keymap=<active scheme name>`)
   can be added by the framework for the main help site only. aIDE has a keymap
   UI, so the active keymap scheme is the obvious one to pass.
6. **Version granularity**: use `major.minor` only, so all patch releases share
   one docs tree.
