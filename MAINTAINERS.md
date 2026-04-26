# Maintainer Guide

## Add contributors with all-contributors bot

see: https://allcontributors.org/en/bot/overview/

In pull requests add a comment in the following format:

```
@all-contributors please add @username for code, doc, design ...
```

## Releasing

Releases are managed automatically by [Release Please](https://github.com/googleapis/release-please):

1. Ensure the latest commit on `develop` passes all CI checks
2. Merge `develop` into `main`
3. Merge the automatically created release pull request after it passes all checks
