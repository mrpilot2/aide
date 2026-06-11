# SettingsInterface

`aide::SettingsInterface` is the abstraction over `QSettings`. It is used throughout aIDE and your application to read and write persistent key-value data addressed by `HierarchicalId`.

**Header:** `aide/settingsinterface.hpp`

---

## The two settings stores

aIDE maintains two separate `SettingsInterface` instances, accessible via `aide::AideSettingsProvider`:

| Store | Accessor | Use for |
|---|---|---|
| **Versionable** | `AideSettingsProvider::versionableSettings()` | Application state that should be reset on a version upgrade (e.g. cached data, layout that changes between versions) |
| **Unversionable** | `AideSettingsProvider::unversionableSettings()` | User preferences that should survive version upgrades (e.g. theme, editor font size) |

```cpp
#include <aide/aidesettingsprovider.hpp>

auto prefs  = aide::AideSettingsProvider::unversionableSettings();
auto cache  = aide::AideSettingsProvider::versionableSettings();
```

Both are `shared_ptr<aide::SettingsInterface>`.

---

## Reading and writing values

Keys are `HierarchicalId`s; values are `QVariant`:

```cpp
auto settings = aide::AideSettingsProvider::unversionableSettings();

// Write
settings->setValue(
    aide::HierarchicalId("MyApp")("Editor")("fontSize"), 14);

// Read (throws if key absent — use the defaultValue overload in practice)
QVariant raw = settings->value(
    aide::HierarchicalId("MyApp")("Editor")("fontSize"));

// Read with default
int fontSize = settings->value(
    aide::HierarchicalId("MyApp")("Editor")("fontSize"),
    12).toInt();
```

### Removing a key

```cpp
settings->removeKey(aide::HierarchicalId("MyApp")("Editor")("fontSize"));
```

---

## Saving and loading

aIDE calls `save()` and `load()` automatically during the settings dialog apply/reset cycle, and on application shutdown. You generally do not need to call these manually. They are exposed on the interface in case you need explicit control:

```cpp
settings->save();  // flush to QSettings backend
settings->load();  // reload from QSettings backend
```

---

## Using settings in a SettingsPage

The standard pattern is to accept a `shared_ptr<SettingsInterface>` in your `SettingsPage` constructor and use it inside `apply()` and `reset()`:

```cpp
class MySettingsPage : public aide::core::SettingsPage
{
public:
    MySettingsPage(aide::HierarchicalId group,
                   std::shared_ptr<aide::SettingsInterface> settings)
        : SettingsPage(std::move(group))
        , m_settings(std::move(settings))
    {}

    void apply() override
    {
        m_settings->setValue(
            aide::HierarchicalId("MyApp")("fontSize"),
            m_ui->fontSizeSpinBox->value());
    }

    void reset() override
    {
        int size = m_settings->value(
            aide::HierarchicalId("MyApp")("fontSize"), 12).toInt();
        m_ui->fontSizeSpinBox->setValue(size);
    }
    // ...
private:
    std::shared_ptr<aide::SettingsInterface> m_settings;
};
```
