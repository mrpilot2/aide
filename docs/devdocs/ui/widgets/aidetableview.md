# AideTableView

`aide::widgets::AideTableView` is a `QTableView` subclass that adds placeholder text support. When the model has no rows, a centred, greyed-out text is displayed in the viewport.

**Header:** `aide/gui/widgets/aidetableview.hpp`

---

## Usage

```cpp
#include <aide/gui/widgets/aidetableview.hpp>

auto* tableView = new aide::widgets::AideTableView(parentWidget);
tableView->setModel(myTableModel);
tableView->setPlaceHolderText(tr("No data available."));
```

---

## API

### `setPlaceHolderText(QString text)`

Sets the text displayed when the model has no rows (or no model is set). The text is centred in the viewport and rendered in a muted colour.

```cpp
tableView->setPlaceHolderText(tr("No entries."));
```

Calling this with an empty string disables the placeholder.

---

## Notes

- `AideTableView` does not change any other `QTableView` behaviour. All standard Qt model/view APIs work as usual.
- The placeholder uses the same `paintEvent` override mechanism as [`AideTreeView`](aidetreeview.md).
