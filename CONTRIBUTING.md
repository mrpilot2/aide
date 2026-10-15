# Contributing to aIDE

## Commit messages

Commit messages must follow [Conventional Commits](https://www.conventionalcommits.org/) (`feat:`, `fix:`, `ci:`, `chore:`, etc.) — enforced by pre-commit hook.

## Documentation

A change to a public header under `aide/src/include/aide/` updates the page under `docs/` that covers that API (e.g. `ApplicationConfig` → `docs/fundamentals/application-config.md`). A new `ApplicationConfig::Feature` adds a row to that page's features table.

## Code formatting

Pre-commit hooks enforce formatting. Run manually with:

```bash
pre-commit run --all-files     # all hooks
pre-commit run clang-format    # C++ only
pre-commit run cmake-format    # CMake only
```