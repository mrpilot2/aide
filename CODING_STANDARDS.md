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
