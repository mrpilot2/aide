--8<-- "CONTRIBUTING.md"

## End-User Help

aIDE ships End-User Help pages for its built-in Help Topics in
[`aide/help/`](https://github.com/mrpilot2/aide/tree/develop/aide/help), one
mkdocs-material page per topic (`appearance.md`, `keymap.md`,
`notifications.md`). They are separate from this Developer Guide: clients
include them into their own help site with `pymdownx.snippets` stubs, so the
same text appears under the client's name, theme and URLs.

Follow these authoring rules for every page in `aide/help/`:

- **Neutral wording.** Refer to "the application", "the Settings dialog" and
  so on. Clients add their own name and specifics around the include.
- **No template variables.** Do not write `{{ ... }}`. Variables are not
  rendered inside snippets includes, and aIDE offers no variable contract.
- **No concrete key sequences.** Name the action as the menus and the Keymap
  show it (e.g. "the **Settings** action") and point to the Keymap page for
  the current shortcut. Never write a key combination such as a modifier plus
  a letter, or a function key: users remap shortcuts, clients change
  defaults, and macOS differs.
- **Never mention aIDE.** Pages must not name aIDE or link to its repository
  or Developer Guide.
- **No relative links or images.** The client decides where each page lands
  in its site, so a relative link or image path breaks after inclusion.

`tools/check-end-user-help.sh` enforces the rules a grep can catch (aIDE,
`{{`, modifier key sequences and function keys); `tools/test-check-end-user-help.sh`
is its self-test. Run both after editing a help page:

```bash
tools/check-end-user-help.sh
tools/test-check-end-user-help.sh
```
