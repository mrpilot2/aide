# API Reference

This section contains the full API reference generated from the aIDE source code using [Doxygen](https://www.doxygen.nl/) and [doxybook2](https://github.com/matusnovak/doxybook2).

!!! info "CI-generated content"
    The pages in this section are generated automatically during the CI build. If you are viewing the docs locally without running doxybook2, only this index page will be present.

    To generate the API reference locally:

    ```bash
    # 1. Generate Doxygen XML
    doxygen docs/Doxyfile

    # 2. Convert to Markdown with doxybook2
    doxybook2 --input build/doxygen/xml \
              --output docs/api-reference \
              --config .doxybook/config.json
    ```

---

## Browse

- [Classes](index_classes.md) — all classes and structs
- [Namespaces](index_namespaces.md) — all namespaces

---

## Namespaces

The entire aIDE public API lives under the `aide` namespace:

| Namespace | Description |
|---|---|
| `aide` | Core types: `Application`, `HierarchicalId`, `ActionRegistryInterface`, `SettingsInterface` |
| `aide::core` | Settings pages, `SettingsPageRegistry`, `LoggerFactory` |
| `aide::gui` | `TranslatorInterface` |
| `aide::widgets` | `AideTreeView`, `AideTableView`, `SearchFilterWidget`, `SearchLineEdit` |
| `aide::constants` | `CONSTANTS()` — well-known `HierarchicalId` values |

---

## Public headers

All public headers are installed under the `aide/` prefix:

```
aide/application.hpp
aide/actionregistryinterface.hpp
aide/aidesettingsprovider.hpp
aide/aideconstants.hpp
aide/hierarchicalid.hpp
aide/loggerinterface.hpp
aide/logger/loggerfactory.hpp
aide/logger/log_helper_macros.hpp
aide/menucontainerinterface.hpp
aide/settingsinterface.hpp
aide/settings/settingspage.hpp
aide/settings/settingspageregistry.hpp
aide/gui/translatorinterface.hpp
aide/gui/widgets/aidetableview.hpp
aide/gui/widgets/aidetreeview.hpp
aide/gui/widgets/searchfilterwidget.hpp
aide/gui/widgets/searchlineedit.hpp
aide/utils/systemmemory.hpp
```
