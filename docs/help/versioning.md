# Versioned Help

aIDE resolves a topic to `<Help Base URL>/<topic-id>/` and nothing else:

- It **never adds a version** to the URL, neither aIDE's nor yours.
- It **never checks a URL at runtime**: no request to see whether a page exists, no fallback to another version.

Your help site is built against one pinned aIDE version (the [snippets stubs](help-site.md) include the pages of the aIDE you link against), so each of your help releases already holds the matching aIDE content. If you version your help, the version is yours and goes into your Help Base URL; unknown topics are handled by your docs server.

The recommended setup uses [`mike`](https://github.com/jimporter/mike), which publishes one copy of an mkdocs site per version.

---

## Step 1: one site per `major.minor`

Deploy each release of your help under your application's `major.minor`, and keep a `latest` alias on the newest one:

```bash
pip install mike
mike deploy --push --update-aliases 2.3 latest
mike set-default --push latest
```

This serves `https://example.com/help/2.3/`, `https://example.com/help/latest/` and a root that redirects to `latest`. Patch releases (`2.3.1`) redeploy `2.3`.

Add mike's version selector to the theme if you like:

```yaml
extra:
  version:
    provider: mike
```

---

## Step 2: point the Help Base URL at that version

Set the base URL from your project version at build time, so each build of the application opens the help written for it:

```cmake
project(myapp VERSION 2.3.1)
target_compile_definitions(
    myapp PRIVATE
    MYAPP_HELP_BASE_URL="https://example.com/help/${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}/"
)
```

```cpp
config.setHelpBaseUrl(QUrl(MYAPP_HELP_BASE_URL));
// keymap -> https://example.com/help/2.3/keymap/
```

Use `latest` in the base URL only if all installed versions of your application should open the newest help.

---

## Step 3: handle unknown topics

An installed application may ask for a topic that the published help no longer has, for example after you moved a page. Since aIDE does not check, give the docs server a fallback:

- **A 404 page.** mkdocs-material builds a `404.html` into each version. Static hosts such as GitHub Pages serve only the `404.html` at the root of the whole site for every missing path, so with `mike` also put one at the root (e.g. a copy of the `latest` version's, or the host's own custom-404 setting). Give it a link to Help Contents and to search, so the user can still find the topic.
- **Redirects.** For pages you moved or renamed, add a redirect from the old path, for example with the [`mkdocs-redirects`](https://github.com/mkdocs/mkdocs-redirects) plugin:

    ```yaml
    plugins:
      - redirects:
          redirect_maps:
            keymap.md: settings/keymap.md
    ```

    Inside the application, `remapHelpTopic()` does the same for newer builds (see [Remapping and disabling topics](index.md#remapping-and-disabling-topics)); the redirect keeps older installed versions working.

The [coverage test](coverage-test.md) makes sure a newly built application never ships with a topic that its own help version lacks; the 404 page and redirects cover the rest.

!!! note "The demo"
    The demo's help site is unversioned: one site at `https://mrpilot2.github.io/aide/demo-help/`, rebuilt on each aIDE release. The `mike` recipe is documented here but not demonstrated by the demo.
