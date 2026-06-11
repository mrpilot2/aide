# SearchLineEdit

`aide::widgets::SearchLineEdit` is a `QLineEdit` subclass that highlights substrings within the input text. It is used internally by [`SearchFilterWidget`](search-filter-widget.md) to mark matched filter terms, but is available independently for any text input where tag-based highlighting is useful.

**Header:** `aide/gui/widgets/searchlineedit.hpp`

---

## Usage

```cpp
#include <aide/gui/widgets/searchlineedit.hpp>

auto* lineEdit = new aide::widgets::SearchLineEdit(parentWidget);
lineEdit->setTags({"error", "warning"});
```

---

## API

### `setTags(QList<QString> tags)`

Specifies the list of substrings to highlight in the line edit's text. Each occurrence of any tag in the current text is drawn with a rounded highlight rectangle.

```cpp
lineEdit->setTags({"TODO", "FIXME", "HACK"});
```

The default highlight colour is a semi-transparent teal (`rgba(0, 96, 100, 150)`). This is a compile-time constant; to change it, subclass `SearchLineEdit` and override `paintEvent`.

Passing an empty list removes all highlights.

---

## Notes

- Highlighting is redrawn on every `paintEvent`. For very long strings with many tags, performance is proportional to the number of character comparisons, but this is negligible for typical line-edit lengths.
- The highlight uses rounded rectangles with a 5 px corner radius.
- Tag matching is substring-based, not regex-based.
