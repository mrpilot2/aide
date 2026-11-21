# Adding Settings Pages

The aIDE settings dialog uses a tree structure where each node is a settings page. You add your own pages by subclassing `aide::core::SettingsPage` and registering them with `SettingsPageRegistry`.

**Headers:**  
- `aide/settings/settingspage.hpp`  
- `aide/settings/settingspageregistry.hpp`

---

## Implementing a SettingsPage

Subclass `aide::core::SettingsPage` and implement the four pure virtual methods:

```cpp
#include <QWidget>
#include <aide/settings/settingspage.hpp>
#include <aide/settingsinterface.hpp>

class MySettingsPage
    : public QObject               // needed if you use Qt signals/slots
    , public aide::core::SettingsPage
{
    Q_OBJECT
public:
    MySettingsPage(aide::HierarchicalId group,
                   std::shared_ptr<aide::SettingsInterface> settings)
        : SettingsPage(std::move(group))
        , m_settings(std::move(settings))
    {}

    ~MySettingsPage() override = default;

    // Return the widget to display in the settings dialog.
    // Created lazily on first call.
    QWidget* widget() override
    {
        if (!m_widget) {
            m_widget = new QWidget();
            // populate m_widget ...
            reset(); // populate fields from settings
        }
        return m_widget;
    }

    // Return true if the user has changed any fields since the last apply/reset.
    bool isModified() const override
    {
        return m_widget && m_spinBox->value() != m_lastAppliedValue;
    }

    // Discard user changes and reload from settings.
    void reset() override
    {
        int value = m_settings->value(
            aide::HierarchicalId("MyApp")("someKey"), 0).toInt();
        m_spinBox->setValue(value);
        m_lastAppliedValue = value;
    }

    // Persist user changes to settings.
    void apply() override
    {
        m_settings->setValue(
            aide::HierarchicalId("MyApp")("someKey"),
            m_spinBox->value());
        m_lastAppliedValue = m_spinBox->value();
    }

private:
    std::shared_ptr<aide::SettingsInterface> m_settings;
    QWidget*     m_widget{nullptr};
    QSpinBox*    m_spinBox{nullptr};
    int          m_lastAppliedValue{0};
};
```

### SettingsPage contract

| Method | When called | Expected behaviour |
|---|---|---|
| `widget()` | When the user navigates to this page | Return the page widget (create it on first call) |
| `isModified()` | On every dialog repaint | Return `true` if the user has unsaved changes |
| `reset()` | Dialog opens; user clicks Cancel or Reset | Reload all fields from the settings store |
| `apply()` | User clicks Apply or OK | Write all fields to the settings store |

---

## Setting the page's position in the tree

The `HierarchicalId` passed to the `SettingsPage` constructor controls where the page appears in the settings tree. Each level becomes a tree node:

```
HierarchicalId("My App")("Editor")("Fonts")
```

produces:

```
My App
└── Editor
    └── Fonts   ← your page
```

You can register multiple pages under the same parent:

```cpp
aide::HierarchicalId editorBase("My App")("Editor");

SettingsPageRegistry::addPage(std::make_unique<FontsPage>(
    editorBase("Fonts"), settings));

SettingsPageRegistry::addPage(std::make_unique<ColorsPage>(
    editorBase("Colors"), settings));
```

---

## Registering the page

Call `SettingsPageRegistry::addPage()` after constructing `aide::Application` and before calling `exec()`:

```cpp
#include <aide/settings/settingspageregistry.hpp>
#include <aide/aidesettingsprovider.hpp>

aide::core::SettingsPageRegistry::addPage(
    std::make_unique<MySettingsPage>(
        aide::HierarchicalId("My App")("Preferences"),
        aide::AideSettingsProvider::unversionableSettings()));
```

!!! tip
    Pass `AideSettingsProvider::unversionableSettings()` for user preferences that should survive version upgrades. Use `versionableSettings()` for ephemeral cached state.

The settings dialog is created lazily the first time the user opens it, so pages registered before `exec()` are always visible.

---

## Using a Qt Designer `.ui` file

The demo application uses a `.ui` file for its settings page widget. The pattern is:

```cpp
#include "ui_mysettingspage.h"

QWidget* MySettingsPage::widget()
{
    if (!m_widget) {
        m_ui = std::make_unique<Ui::MySettingsPage>();
        m_widget = new QWidget();
        m_ui->setupUi(m_widget);
        reset();
    }
    return m_widget;
}
```

See [`demo/src/demosettingspage.cpp`](https://github.com/mrpilot2/aide/blob/main/demo/src/demosettingspage.cpp) for a complete working example.
