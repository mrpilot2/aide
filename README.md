# aide

A modern, C++ native GUI application framework based on QT5 - using C++11 and later

![CI Build and test](https://github.com/mrpilot2/aide/workflows/CI%20-%20Build%20and%20Test/badge.svg)
![CI Build and install](https://github.com/mrpilot2/aide/workflows/Build%20standalone%20and%20test%20installed%20lib/badge.svg)

[![codecov](https://codecov.io/gh/mrpilot2/aide/branch/develop/graph/badge.svg)](https://codecov.io/gh/mrpilot2/aide)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/6f3c6536d8184f7baec9cc37e4bddaad)](https://app.codacy.com/manual/mrpilot2/aide?utm_source=github.com&utm_medium=referral&utm_content=mrpilot2/aide&utm_campaign=Badge_Grade_Settings)
[![CodeQL](https://github.com/mrpilot2/aide/workflows/CodeQL/badge.svg)](https://github.com/mrpilot2/aide/actions/workflows/codeql.yml)

<!-- ALL-CONTRIBUTORS-BADGE:START - Do not remove or modify this section -->
[![All Contributors](https://img.shields.io/badge/all_contributors-1-orange.svg?style=flat-square)](#contributors-)
<!-- ALL-CONTRIBUTORS-BADGE:END -->

## Table of contents

-   [Contributors](#contributors-)
-   [License](#license)
-   [Development](#development)

## Contributors ✨

This project follows the [all-contributors](https://github.com/all-contributors/all-contributors) specification and is 
developed by these [contributors](https://github.com/mrpilot2/aide/blob/develop/Contributors.md).
Contributions of any kind welcome!

## License

This project is licensed under the terms of the [GPLv3](https://github.com/mrpilot2/aide/blob/develop/LICENSE).

However, aIDE includes several third-party Open-Source libraries, which are licensed under their own respective Open-Source licenses.
These licenses can be found in the [3rd Party Licenses](https://github.com/mrpilot2/aide/blob/develop/3rd-party-licenses.md)

## Development

### How to release

This project uses [Release please](https://github.com/googleapis/release-please) GitHub action to automatically create releases.

The following steps are required to create a release:
1.  Make sure the latest commit on ``develop`` passes all checks
2.  Merge the latest change from ``develop`` into ``main`` branch
3.  Merge the automatically created pull request after it passes all checks
