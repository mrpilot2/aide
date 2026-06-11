# Translations

aIDE integrates Qt's `QTranslator` system and provides a `TranslatorInterface` that lets you register additional translation files alongside aIDE's own translations.

**Header:** `aide/gui/translatorinterface.hpp`  
**Access:** `app.translator()` → `shared_ptr<TranslatorInterface>`

---

## How translations work

aIDE loads translations automatically based on the system locale at startup. Your application can add its own `.qm` files by registering additional translation file paths before `exec()`.

---

## Adding your translation files

```cpp
#include <QDir>
#include <aide/gui/translatorinterface.hpp>

app.translator()->addAdditionalTranslationFilePath(
    QDir(":/my_translations"),  // QDir pointing to the directory containing .qm files
    QString("my_app"));         // base file name (without locale suffix and .qm extension)
```

aIDE will look for files of the form `<fileName>_<locale>.qm` in the given directory and load the one matching the current system locale.

### Using Qt resources

The recommended approach is to embed translation files in a Qt resource file (`.qrc`):

```xml
<!-- my_translations.qrc -->
<RCC>
    <qresource prefix="/my_translations">
        <file>my_app_en.qm</file>
        <file>my_app_de.qm</file>
    </qresource>
</RCC>
```

Then register:

```cpp
app.translator()->addAdditionalTranslationFilePath(
    QDir(":/my_translations"), QString("my_app"));
```

---

## Querying available translations

To retrieve the set of locales for which translations are available (useful for a language selection UI):

```cpp
std::set<std::string> available = app.translator()->getAvailableTranslations();
for (const auto& locale : available) {
    // e.g. "en", "de", "fr"
}
```

---

## Creating `.qm` files

Qt translation follows the standard workflow:

1. Mark strings in source code with `QObject::tr()` or `QApplication::translate()`
2. Run `lupdate` to extract strings into a `.ts` file
3. Translate the `.ts` file with Qt Linguist
4. Run `lrelease` to compile the `.ts` file to a `.qm` file
5. Embed the `.qm` file in a Qt resource file

See the demo application's `demo/src/res/` directory for a complete example with `demo_en.ts` and `demo_de.ts`.

---

## Demo example

```cpp
// From demo/src/main.cpp
app.translator()->addAdditionalTranslationFilePath(
    QDir(":/demo_translations"), QString("demo"));
```

The demo embeds `demo_en.qm` and `demo_de.qm` in the `:/demo_translations` resource path. aIDE picks the correct one based on the system locale.
