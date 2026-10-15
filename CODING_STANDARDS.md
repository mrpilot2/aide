# Coding Standards

Judgement calls for review. Anything formatting, clang-tidy or cppcheck enforces lives in `.clang-format` / `.clang-tidy` and the `dev` preset, not here.

## Tests

### Shared test doubles

Test doubles live in `tests/ut/` as shared files: `mock*`, `spy*`, `nulllogger`, `recordinglogger`, `capturedmacrolog`. When a test needs a double for an interface that already has one, extend the shared double (a new flag, setter or recorded value) and reuse it.

Flag: a test-local class in an anonymous namespace that implements an interface which already has a shared double, or a near-copy of one that already exists in another test file.

**Why:** when an interface gains a method, every copy has to be edited, and the copies drift apart in what they record.

### Doubles must not stand in for the behaviour under test

When a test checks a default, fallback or base-class behaviour, the object under test must actually run that code. A double that overrides the method being tested makes the test pass by construction.

Flag: a test named for a default (e.g. "a page that does not declare X") whose double overrides X. A shared double's override should fall back to the base implementation when nothing is configured (e.g. `value_or(Base::method())`), or the test should use a minimal subclass that doesn't override it.

**Why:** in #195, a test of `SettingsPage::helpTopicId()`'s empty default used a `MockSettingsPage` that overrode `helpTopicId()`, so the base default was never run.

### Test behaviour, not pixels

GUI tests assert on state and signals: `QSignalSpy` for emitted signals, `findChild<T*>("objectName")` for child widget state, `QTest` for input. Painting paths get smoke coverage (show, repaint, no crash), not pixel or screenshot comparison. Blocking `exec()` calls (menus, dialogs) are closed from a `QTimer::singleShot(0, …)` queued before the call.

Flag: a test that grabs a widget's pixmap and compares it, or that finds a widget through `QApplication::topLevelWidgets()` when it is (or could become) a child of a known window — search that window's `findChildren()` instead.

**Why:** rendering differs between X11, Wayland, Windows, macOS and themes. In 374422ef a test broke because a tooltip it looked up in `topLevelWidgets()` had been reparented.

## Qt object model

### One owner per QObject

A `QObject` with a parent belongs to that parent: hold it as a raw pointer, and don't also wrap it in `std::unique_ptr` or `std::shared_ptr`. A `QObject` you don't own but need to reach later is held as a `QPointer` and checked for null before each use. Smart pointers are for objects with no parent.

Flag: `std::make_unique<QWidget>(this)`-style constructs (smart pointer plus parent), a raw pointer member to a `QObject` the class doesn't own and doesn't track, or an API that makes callers keep a `shared_ptr` alive only to satisfy a `weak_ptr` inside it.

**Why:** two owners means a double delete when the parent dies first. In 5f81cfef `ActionRegistry` moved from `weak_ptr<QAction>` to `QPointer<QAction>` because every action was already parented and the `weak_ptr` bookkeeping added dangling-`lock()` risk.

### Lambda connections need a context object

A `connect` to a lambda that captures `this` (or any other object) passes a context object as the third argument, normally the object whose members the lambda touches: `connect(sender, &Sender::sig, this, [this] { … })`.

Flag: the context-less overload `connect(sender, &Sender::sig, [this] { … })` where the captured object can be destroyed before `sender`.

**Why:** without a context, the connection lives as long as the sender, and the lambda runs against a destroyed `this`. With one, Qt drops the connection when the context dies and runs the lambda in the context's thread.

### String-based connect across aIDE libraries

A `connect` whose sender and receiver are `QObject` classes from *different* aIDE libraries (`AideCore`, `AideGui`, `AideUtils`, `AideLogger`, the application library, or a test binary against any of them) uses the `SIGNAL()`/`SLOT()` string form, with the rationale comment copied from `aide/src/applicationbuilder.cpp`. Pointer-to-member connects are fine within one library and to Qt's own classes.

Flag: a new pointer-to-member connect between classes from two aIDE libraries.

**Why:** the pointer-to-member form references `staticMetaObject`, a data symbol, and `CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS` exports only functions, so it fails to link on MSVC shared builds. Only the Windows shared CI job catches this; Linux builds pass.

### Process-global state is per module

In shared builds on Windows and macOS, each aIDE library and each test binary gets its own copy of every global and static: the spdlog registry, function-local statics, singletons. Code outside `AideLogger` registers and drops loggers through `LoggerMacros`, never `spdlog::` directly. New global state needs a single owner library and an exported accessor.

Flag: `spdlog::register_logger`, `spdlog::get` or `spdlog::drop` outside `aide/src/logger/`, or a header-defined `static`/`inline` variable that is meant to be shared across libraries.

**Why:** in PR #204 a test passed on Linux (ELF merges the copies) and failed on Windows/macOS shared CI because it registered its logger in its own registry.

## Layering

### Use cases talk to views through interfaces

Business logic in `core/` (use cases, controllers) reaches the GUI through a view interface that `gui/` implements, e.g. `ApplicationCloseView` / `ApplicationCloseController`. A use case doesn't construct dialogs, call `QMessageBox`, or `exec()` anything.

Flag: new `QDialog`, `QMessageBox` or widget construction inside a `core/` use case or controller. Some existing `core/` files include widget headers; don't add to them.

**Why:** a view interface lets the use case be tested with a spy and no event loop; a modal `exec()` in core makes it untestable without a `QTimer` hack.

### Public headers stay lean

Headers under `aide/src/include/aide/` are the consumer API. They forward-declare what they can, keep implementation detail behind a pimpl (`ApplicationConfig::Impl`) or in `aide/src/`, and don't include third-party headers (the logging macros in `logger/log_helper_macros.hpp` are the deliberate exception).

Flag: a new third-party include or a private-header include in a public header, or a new data member on a public class that could live in the pimpl.

**Why:** every include in a public header becomes a compile-time dependency for every consumer, and every data member change breaks their ABI.

## Strings and i18n

### Translate text, never identifiers

Text a user reads goes through `tr()` (or `QT_TR_NOOP` in static tables, translated where it's used). Identifiers never do: `HierarchicalId`s, settings keys, `objectName`s, help topic IDs, log messages. Non-translatable literals are `QStringLiteral(…)`.

Flag: a bare `"…"` or `QStringLiteral` shown in a label, button, menu, tooltip or dialog; or `tr()` around a key or ID.

**Why:** untranslated UI text ships in English to every locale and nothing catches it; a translated key silently changes when the language changes and loses the stored setting.

### Don't let `auto` hold a QStringBuilder

Concatenations with `%` (or `+` under `QT_USE_QSTRINGBUILDER`) produce a lazy expression holding references to the operands. Store the result as `QString`, not `auto`.

Flag: `auto s = a % b;` or `auto s = a + b;` with `QString` operands.

**Why:** the expression outlives temporaries it refers to, and reading it later is a use-after-free that ASAN catches only if the test happens to run that path.

### Don't detach Qt containers in range-for

Range-for over a non-const Qt container (`QList`, `QString`, `QHash`, …) that may be shared uses `std::as_const(container)`, or iterates a `const` reference.

Flag: `for (const auto& x : m_list)` inside a non-const member function, or over a non-const local copied from elsewhere.

**Why:** Qt containers are implicitly shared; calling non-const `begin()` on a shared one deep-copies it. clang-tidy doesn't flag this.

## Item models

### Every model mutation is bracketed

A change to an item model's backing data is wrapped in the matching `begin…`/`end…` pair (`beginInsertRows`, `beginRemoveRows`, `beginMoveRows`, `beginResetModel`, and on Qt ≥ 6.10 `beginFilterChange`/`endFilterChange` in proxies). `dataChanged` names the exact range and the roles that changed.

Flag: backing data changed with no `begin…` call, a `begin…` whose indices don't match what's then changed, `layoutChanged` used for insert/remove, or `dataChanged` with an empty role list when only some roles changed.

**Why:** views and proxies cache persistent indexes; an unbracketed change corrupts them and crashes later, far from the cause.

## GUI and platforms

### No global coordinates for popups

Popups, balloons and tooltips anchored to a widget are child widgets of that widget (or its window), positioned in its local coordinates. Top-level `Qt::Tool`/`Qt::ToolTip` windows placed with `mapToGlobal()` + `move()` are out.

Flag: a new top-level window positioned in screen coordinates to sit next to another widget.

**Why:** Wayland gives clients no global coordinate space, so the compositor puts such windows somewhere else. Fixed in fd4d20b3 for `NotificationBalloon` and `GotItTooltip`.

### Colours come from the palette

Foreground and background colours derive from the widget's `QPalette` roles (adjusted with `lighter()`/`darker()` depending on whether the theme is dark), or come as an explicit light/dark pair. A single fixed colour for text or a surface is not enough.

Flag: a hard-coded `QColor` used as a background or text colour without a dark-theme counterpart.

**Why:** in c0bc8730 a fixed pastel balloon background stayed light under a dark theme while the text followed the theme, making it unreadable.

## C++20

### Exceptions are for broken contracts only

`throw` is reserved for programmer errors that make continuing meaningless, raised at setup time: an `Application` created without an organisation name, a duplicate `SettingsPageRegistry` registration, a null model that can't be null. Anything that can fail at runtime with correct code (a missing file, an unknown setting value, a bad user input) returns a value (`std::optional`, `bool` plus a logged reason, a small result struct) or emits a signal.

Flag: a `throw` for a recoverable runtime condition, a `throw` reachable from a slot, event handler or model/delegate override, or a call into a throwing API (`std::stoi`, `std::filesystem` without the `error_code` overload, `.at()`) on a runtime path with no guard.

**Why:** Qt's event loop isn't exception-safe. An exception escaping a slot or event handler terminates the consumer's application instead of reaching a handler.

### Constrain templates with concepts

New templates state their requirements with a concept or `requires` clause, not `std::enable_if` or an unconstrained `typename T`.

Flag: `std::enable_if`/`std::void_t` SFINAE in new code, or a template that only works for a few types but accepts any.

**Why:** constraints show up in the signature and give a one-line error at the call site instead of a page of instantiation backtrace.
