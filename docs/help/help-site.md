# Building the Help Site

Your End-User Help is your own mkdocs-material site, served at your [Help Base URL](index.md#turning-help-on). aIDE ships the pages for its [built-in Help Topics](index.md#built-in-help-topics) (`appearance.md`, `keymap.md`, `notifications.md`); you pull them into your site with small **stub** pages that include them through `pymdownx.snippets`. The text then appears under your application's name, theme and URLs, and it always matches the aIDE version you link against.

The demo application's help site in [`demo/help/`](https://github.com/mrpilot2/aide/tree/develop/demo/help) is the reference for this recipe: copy its `mkdocs.yml`, `requirements.txt` and stubs as a starting point.

---

## Step 1: get aIDE's help directory from CMake

The CMake variable **`AIDE_HELP_DIR`** holds the absolute path of the directory with aIDE's pages, however you consume aIDE:

| Consumption | Where `AIDE_HELP_DIR` comes from | Points to |
|---|---|---|
| `FetchContent` / `add_subdirectory` | cache entry set while aIDE is configured; visible from every directory after `FetchContent_MakeAvailable(aide)` | `<aide source>/aide/help` |
| `find_package(AideQt6)` | set by the installed package config | `<prefix>/share/aide/help` |

The installed pages belong to the `Help` install component. To install only the pages:

```bash
cmake --install <build-dir> --component Help --prefix <prefix>
```

The installed package config sets `AIDE_HELP_DIR` even if the `Help` component was not installed, so the variable can point to a directory that does not exist. The docs build then fails loudly on the first include (see `check_paths` below) rather than producing empty pages; install the `Help` component to fix it.

Hand the directory to the docs build as an environment variable, for example from a custom target:

```cmake
add_custom_target(
    help-site
    COMMAND ${CMAKE_COMMAND} -E env "AIDE_HELP_DIR=${AIDE_HELP_DIR}"
            mkdocs build --strict -d ${CMAKE_BINARY_DIR}/help-site
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/help
)
```

Outside CMake (e.g. in CI), set `AIDE_HELP_DIR` yourself. The demo site is built from the aIDE repository root with:

```bash
python3 -m venv build/help-venv
build/help-venv/bin/pip install -r demo/help/requirements.txt
AIDE_HELP_DIR="$PWD/aide/help" build/help-venv/bin/mkdocs build --strict \
  -f demo/help/mkdocs.yml -d "$PWD/build/demo-help-site"
```

---

## Step 2: configure snippets

In your site's `mkdocs.yml`, point `pymdownx.snippets` at the directory and turn on `check_paths`:

```yaml
site_url: https://example.com/help/   # your Help Base URL
use_directory_urls: true              # mkdocs' default; Help Topic URLs depend on it

theme:
  name: material

markdown_extensions:
  - tables
  - pymdownx.snippets:
      base_path:
        - !ENV AIDE_HELP_DIR
      check_paths: true
```

- `base_path` makes `--8<-- "keymap.md"` resolve against aIDE's help directory.
- `check_paths: true` turns a missing include into an error, so a wrong or unset `AIDE_HELP_DIR`, or a page aIDE no longer ships, fails the build instead of producing an empty page. With `AIDE_HELP_DIR` unset, mkdocs stops with a configuration error.
- `use_directory_urls: true` makes each page `<topic-id>/index.html`, served at `<Help Base URL>/<topic-id>/`, which is exactly where aIDE opens it. Place each page at the path of its Help Topic ID: `docs/keymap.md` for `keymap`, `docs/settings/keymap.md` for `settings/keymap`.

Build with `mkdocs build --strict`. aIDE's pages use only plain Markdown and tables, so the `tables` extension is the only other one they need.

---

## Step 3: write the stubs

For each built-in topic, choose one of four options:

=== "Include"

    Take aIDE's page as it is. The stub contains only the include:

    ```markdown
    --8<-- "keymap.md"
    ```

=== "Wrap"

    Add your own text before or after the include, for example your application's specifics:

    ```markdown
    --8<-- "keymap.md"

    ## Shortcuts specific to Acme Studio

    The **Build** action is bound to its own shortcut; see the Keymap page.
    ```

    aIDE's pages start with their own `#` heading, so put the include first if you want to keep that heading as the page title.

=== "Override"

    Write your own page at the same path, with no include. The topic keeps working; only the text is yours.

=== "Opt out"

    Create no stub. Then also call `disableHelpTopic()` for that topic (see [Remapping and disabling topics](index.md#remapping-and-disabling-topics)), so the Help button and Context Help don't open a page that does not exist; the [coverage test](coverage-test.md) catches it if you forget.

To place a built-in page somewhere else in your site, put the stub at the new path and remap the topic, e.g. `remapHelpTopic(HelpTopicId{"keymap"}, HelpTopicId{"settings/keymap"})` with the stub in `docs/settings/keymap.md`.

Add a page for each of your own topics too: one per `SettingsPage::helpTopicId()` and per topic passed to `declareHelpTopic()`. Give the site an `index.md`: it is the Help Contents page, opened by the Help menu and by Context Help where no topic applies.

!!! note "What aIDE's pages promise"
    aIDE's pages use neutral wording ("the application", "the Settings dialog"), name actions instead of key sequences (users remap shortcuts, and you may change defaults), never mention aIDE, and contain no relative links or images, so they read correctly wherever you place them. They have no template variables; add application-specific text by wrapping them. Their content changes between aIDE releases; the topic IDs do not. The authoring rules are in [Contributing](../contributing.md#end-user-help).

---

## Next steps

- Check in a test that every topic has a page: [Help Coverage Test](coverage-test.md).
- Publish one help site per release: [Versioned Help](versioning.md).
