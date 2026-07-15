# Appearance

aIDE manages the application's **theme** (color palette), **font**, and **icon theme** through a single component, `aide::AppearanceManager`. It ships with three built-in themes — **System**, **Light**, and **Dark** — automatically follows the operating system's dark/light mode, persists the user's choice across runs, and exposes a ready-made **Appearance** settings page.

**Headers:**
- `aide/appearancemanager.hpp`
- `aide/theme.hpp`
- `aide/colorscheme.hpp`

---

## Overview

`AppearanceManager` is owned by `ApplicationBuilder` and is constructed automatically — you never instantiate it yourself. Access it through the `Application`:

```cpp
aide::AppearanceManager& appearance = app.appearanceManager();
```

On construction it:

1. Registers the three built-in themes (`System`, `Light`, `Dark`).
2. Restores the last-used theme, font family, and font size from the settings store (defaulting to `System` and the current application font).
3. Applies that palette, icon theme, and font to the running `QApplication`.

The builder also registers the [Appearance settings page](#the-appearance-settings-page) and connects the manager's `appearanceChanged()` signal to the main window's icon refresh, so theme changes made by the user take effect immediately.

---

## Themes

A theme is a plain aggregate, `aide::Theme`:

```cpp
struct Theme
{
    QString     name;            // unique, shown in the settings combo box
    QPalette    palette;         // applied via QApplication::setPalette()
    QString     iconThemeName;   // QIcon::setThemeName() value
    QStringList iconSearchPaths; // QIcon::setThemeSearchPaths() value
};
```

### Built-in themes

| Name | Palette | Icon theme | Behaviour |
|---|---|---|---|
| **System** | Qt default (`QPalette{}`) | chosen automatically from the OS color scheme | Follows the OS and reacts to runtime dark/light switches |
| **Light** | bundled light palette | `aide-dark` (dark icons on light background) | Fixed light appearance |
| **Dark** | bundled dark palette | `aide-light` (light icons on dark background) | Fixed dark appearance |

!!! note
    The icon theme is intentionally the *opposite* tone of the palette: a light UI needs dark icons for contrast, and vice versa. For the **System** theme aIDE measures the perceived luminance of the active window background and picks `aide-dark` or `aide-light` accordingly.

### Registering a custom theme

Add your own theme before the user opens the settings dialog (typically right after constructing the `Application`):

```cpp
QPalette bluePalette;
bluePalette.setColor(QPalette::Window, QColor(200, 220, 255));
bluePalette.setColor(QPalette::WindowText, Qt::black);
bluePalette.setColor(QPalette::Base, Qt::white);
// ... fill in the remaining roles ...

app.appearanceManager().registerTheme(
    {"Demo Blue", bluePalette, "aide-dark", {":/aide/icons"}});
```

Registered themes appear automatically in the Appearance settings page combo box. Registering a theme whose `name` already exists throws `std::invalid_argument`.

!!! warning "Magic numbers"
    `dev` and `ci-static` builds treat `readability-magic-numbers` as an error. Extract every `QColor` component into a named `constexpr` before it appears in source. See *Common pitfalls* in `CLAUDE.md`.

---

## Following the OS color scheme

When the active theme is **System**, `AppearanceManager` connects to `QStyleHints::colorSchemeChanged`. If the user switches their OS between light and dark mode while the app is running, the palette and icon theme are re-applied live and the appropriate signals are emitted.

The connection is wired up only while **System** is active; selecting a fixed theme disconnects it.

---

## Reacting to appearance changes

`AppearanceManager` emits two signals:

| Signal | Emitted when |
|---|---|
| `colorSchemeChanged(aide::ColorScheme)` | the resolved light/dark scheme actually flips |
| `appearanceChanged()` | any time the theme, font, or icon settings are applied |

`aide::ColorScheme` is a simple enum:

```cpp
enum class ColorScheme { Light, Dark };
```

Connect to `colorSchemeChanged` when your application has its own resources that depend on the light/dark tone — for example a second icon set:

```cpp
QObject::connect(
    &app.appearanceManager(), SIGNAL(colorSchemeChanged(aide::ColorScheme)),
    &reactor, SLOT(onColorSchemeChanged(aide::ColorScheme)));
```

!!! tip "Use string-based connect across DLL boundaries"
    aIDE connects to these signals with the string-based `SIGNAL()`/`SLOT()` syntax on purpose. The pointer-to-member form references the sender's and receiver's `staticMetaObject` data symbols, which `CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS` does **not** export across DLL boundaries on MSVC shared builds. The string form resolves the connection at runtime through the exported virtual `metaObject()`. Prefer it when connecting to `AppearanceManager` from consumer code that may be built as a shared library. See `demo/src/colorschemereactor.hpp` for a worked example.

---

## Querying and applying appearance

```cpp
QStringList    themeNames();       // all registered theme names
QString        activeThemeName();  // currently applied theme
QFont          activeFont();       // currently applied font
ColorScheme    colorScheme();      // resolved Light/Dark of the active theme

void applyAppearance(const QString& themeName,
                     const QString& fontFamily,
                     int fontSizePoints);
```

`applyAppearance()` is the single entry point that changes the running appearance. It:

1. Looks up the theme (throwing `std::invalid_argument` if unknown).
2. Sets the application palette, icon theme, and font.
3. Persists the theme, font family, and font size to settings.
4. Emits `colorSchemeChanged` (only if the scheme flipped) and `appearanceChanged`.

You rarely call it directly — the Appearance settings page does it for you — but it is available for programmatic theme switching.

---

## Icons

`AppearanceManager` drives Qt's icon theme system via `QIcon::setThemeName()` and `QIcon::setThemeSearchPaths()`. aIDE bundles two icon themes, `aide-light` and `aide-dark`, under the `:/aide/icons` resource prefix.

To add your own icons for a theme, append a search path:

```cpp
app.appearanceManager().addIconSearchPath("System", ":/myapp/icons/dark");
```

For `QIcon::fromTheme()` to resolve icons from an embedded `.qrc`, the resource layout must be:

```
<qrc-prefix>/<theme-name>/index.theme
<qrc-prefix>/<theme-name>/scalable/<category>/<icon-name>.svg
```

where `<qrc-prefix>` matches the path passed to `addIconSearchPath()`. When the appearance changes, the main window's existing `QAction` icons are re-resolved automatically (the builder connects `appearanceChanged()` to `MainWindow::refreshIcons()`).

---

## The Appearance settings page

`ApplicationBuilder` registers `aide::gui::AppearancePage` into the [settings dialog](settings/adding-settings-pages.md) automatically — it appears under **Appearance**. The page lets the user choose:

- the **theme** (any registered theme, including custom ones),
- the **font family** (via a `QFontComboBox`),
- the **font size** (via a `QSpinBox`).

Clicking **Apply** or **OK** calls `applyAppearance()`, which persists the choice and updates the live application. Because the page is registered before `exec()`, it is always present — you do not need to add it yourself.

---

## Settings keys

Appearance state is stored under the `Appearance` group using `versionableSettings()`:

| Key | Type | Default |
|---|---|---|
| `Appearance/Theme` | `QString` | `"System"` |
| `Appearance/Font/Family` | `QString` | current application font family |
| `Appearance/Font/Size` | `int` | current application font point size |

---

## See also

- [Adding Settings Pages](settings/adding-settings-pages.md) — how the Appearance page fits into the settings tree
- [Main Window](../ui/main-window.md) — icon refresh on appearance changes
- [Demo Application](../demo/walkthrough.md) — registers a *Demo Blue* theme and reacts to OS scheme changes
