# AideTreeView

`aide::widgets::AideTreeView` is a `QTreeView` subclass that adds placeholder text support. When the model has no items, a centred, greyed-out text is displayed in the viewport instead of an empty widget.

**Header:** `aide/gui/widgets/aidetreeview.hpp`

---

## Usage

```cpp
#include <aide/gui/widgets/aidetreeview.hpp>

auto* treeView = new aide::widgets::AideTreeView(parentWidget);
treeView->setModel(myModel);
treeView->setPlaceHolderText(tr("No items yet."));
```

---

## API

### `setPlaceHolderText(QString text)`

Sets the text displayed when the model is empty (or no model is set). The text is centred in the viewport and rendered in a muted colour.

```cpp
treeView->setPlaceHolderText(tr("No results found."));
```

Calling this with an empty string disables the placeholder.

---

## Notes

- `AideTreeView` does not change any other `QTreeView` behaviour. All standard Qt model/view APIs (`setModel`, `setRootIndex`, `header`, etc.) work as usual.
- The placeholder is drawn via a `paintEvent` override. It is repainted automatically when the view is resized or the model changes.
