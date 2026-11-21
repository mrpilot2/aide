# Maintainer Guide

## Add contributors with all-contributors bot

see: https://allcontributors.org/en/bot/overview/

In pull requests add a comment in the following format:

```
@all-contributors please add @username for code, doc, design ...
```

## Releasing

Releases are managed automatically by [Release Please](https://github.com/googleapis/release-please):

All development happens on `main`. Every push to `main` updates (or opens) a release pull request with the next version and changelog, derived from the Conventional Commits since the last release.

1. Ensure the latest commit on `main` passes all CI checks
2. Merge the automatically created release pull request after it passes all checks
3. Release Please then tags the release (`vX.Y.Z`) and publishes the GitHub release

Configuration lives in `release-please-config.json`; the current released version is tracked in `.release-please-manifest.json`. The release pull request also bumps the `project(... VERSION ...)` lines marked `# x-release-please-version` in `CMakeLists.txt` and `aide/CMakeLists.txt` — don't edit those versions by hand.
