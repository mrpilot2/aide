# Contributing to aIDE

## Commit messages

Commit messages must follow [Conventional Commits](https://www.conventionalcommits.org/) (`feat:`, `fix:`, `ci:`, `chore:`, etc.) — enforced by pre-commit hook.

## Code formatting

Pre-commit hooks enforce formatting. Run manually with:

```bash
pre-commit run --all-files     # all hooks
pre-commit run clang-format    # C++ only
pre-commit run cmake-format    # CMake only
```