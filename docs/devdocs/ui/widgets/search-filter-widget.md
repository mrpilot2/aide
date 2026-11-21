# SearchFilterWidget

`aide::widgets::SearchFilterWidget` is a composite widget that provides a search bar with live filtering for any `QAbstractItemModel`. It supports case-sensitive matching, regular expression search, and a togglable show/hide shortcut.

**Header:** `aide/gui/widgets/searchfilterwidget.hpp`

---

## Construction

```cpp
#include <aide/gui/widgets/searchfilterwidget.hpp>
#include <aide/hierarchicalid.hpp>

auto* searchWidget = new aide::widgets::SearchFilterWidget(
    aide::HierarchicalId("MyApp")("MyView")("SearchFilter"),  // settings key prefix
    QKeySequence(Qt::CTRL | Qt::Key_F),                        // show/hide shortcut
    parentWidget);
```

**Parameters:**

| Parameter | Type | Description |
|---|---|---|
| `id` | `HierarchicalId` | Prefix for storing widget state (visibility, case-match, regex) in `QSettings` |
| `showHideShortcut` | `QKeySequence` | Keyboard shortcut that toggles the widget's visibility |
| `parent` | `QWidget*` | Parent widget |

The `id` is used to persist the user's last-used filter options (case sensitivity, regex mode) across sessions, scoped by the provided `HierarchicalId`.

---

## Attaching a model

```cpp
searchWidget->setSourceModel(myModel);
```

The widget creates an internal `MultiColumnSortFilterProxyModel`. After calling `setSourceModel`, use `getFilterModel()` to obtain the proxy and set it on your view:

```cpp
auto* proxyModel = searchWidget->getFilterModel();  // QSortFilterProxyModel*
myTableView->setModel(proxyModel);
```

---

## Custom item delegate

The widget provides a delegate that highlights matched text in the filter results:

```cpp
QAbstractItemDelegate* delegate = searchWidget->getItemDelegate();
myTableView->setItemDelegate(delegate);
```

---

## Custom search icon

```cpp
searchWidget->setSearchIcon(QIcon(":/icons/search.svg"));
```

---

## Built-in features

The widget includes the following controls, all persisted per `HierarchicalId`:

| Control | Description |
|---|---|
| Search text field | Filters the model with a 300 ms typing delay |
| Match case checkbox | Toggles case-sensitive matching |
| Regex checkbox | Toggles regular expression mode |
| Show/hide shortcut | Toggles widget visibility (keyboard shortcut set at construction) |

The 300 ms typing delay avoids filtering on every keystroke and makes the widget usable with large models.

---

## Full example

```cpp
auto* searchWidget = new aide::widgets::SearchFilterWidget(
    aide::HierarchicalId("MyApp")("LogView")("Filter"),
    QKeySequence(Qt::CTRL | Qt::Key_F),
    this);

searchWidget->setSourceModel(m_logModel);
searchWidget->setSearchIcon(style()->standardIcon(QStyle::SP_FileDialogContentsView));

m_tableView->setModel(searchWidget->getFilterModel());
m_tableView->setItemDelegate(searchWidget->getItemDelegate());

auto* layout = new QVBoxLayout(this);
layout->addWidget(searchWidget);
layout->addWidget(m_tableView);
```
