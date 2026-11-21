# Key Bindings

aIDE provides a built-in key binding system. Any action registered with the action registry can be assigned default key sequences, and users can override these through the **Keymap** page in the settings dialog.

---

## How key bindings work

When you register an action with one or more default key sequences, aIDE:

1. Applies the default sequences to the `QAction` immediately
2. Checks user settings for a persisted override; if found, applies it instead
3. Exposes the action in the **Keymap** settings page

The Keymap settings page is automatically added to the settings dialog — you do not need to register it yourself.

---

## Registering an action with a default shortcut

```cpp
auto actionSave = std::make_shared<QAction>(QObject::tr("Save"));

app.actionRegistry()->registerAction(
    actionSave,
    aide::HierarchicalId("Main Menu")("File")("Save"),
    "Save the current document",
    {QKeySequence(Qt::CTRL | Qt::Key_S)});
```

Multiple default sequences are supported:

```cpp
app.actionRegistry()->registerAction(
    actionSave,
    aide::HierarchicalId("Main Menu")("File")("Save"),
    "Save the current document",
    {QKeySequence(Qt::CTRL | Qt::Key_S),
     QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_S)});
```

---

## Actions without a default shortcut

Actions registered without key sequences still appear in the Keymap page. Users can assign shortcuts to them from there:

```cpp
app.actionRegistry()->registerAction(
    actionBuild,
    aide::HierarchicalId("Main Menu")("Build")("Build All"),
    "Build all targets");
```

---

## Querying active key sequences

To get the currently active sequences for an action (user override if set, otherwise defaults):

```cpp
const auto& actions = app.actionRegistry()->actions();
auto it = actions.find(aide::HierarchicalId("Main Menu")("File")("Save"));
if (it != actions.end()) {
    QList<QKeySequence> activeSeqs = it->second.getActiveKeySequences();
}
```

---

## Modifying shortcuts programmatically

Override the shortcut for an action at runtime:

```cpp
app.actionRegistry()->modifyShortcutsForAction(
    aide::HierarchicalId("Main Menu")("File")("Save"),
    {QKeySequence(Qt::CTRL | Qt::Key_S)});
```

This persists the new shortcut to the user's settings file. Pass an empty list to reset to the registered default.

---

## The Keymap settings page

The built-in Keymap page is automatically registered under `Settings → Keymap`. It shows all registered actions grouped by their `HierarchicalId` path and lets users:

- View the current (effective) key sequence for each action
- Add new sequences
- Remove individual sequences
- Reset an action to its default sequence

No code changes are needed to make your actions appear there — registering the action is sufficient.
