# aIDE Developer Guide

**aIDE** is a C++20/Qt framework library for building Qt-based desktop applications. It eliminates the boilerplate of a Qt desktop application — giving you a pre-wired action registry, settings dialog, configurable keybindings, and structured logging out of the box, so you can focus on your application's unique functionality.

Think of it as the [IntelliJ Platform](https://www.jetbrains.com/opensource/idea/), but for C++20 and Qt.

---

## What aIDE provides

| Feature | Description |
|---|---|
| **Action Registry** | Register `QAction`s under a `HierarchicalId` and look them up or bind them to menus anywhere in your application |
| **Settings Dialog** | A tree-structured settings UI; add your own pages with a single `SettingsPageRegistry::addPage()` call |
| **Configurable Keybindings** | Per-action key sequences stored in `QSettings`, editable through the built-in keymap settings page |
| **Structured Logging** | spdlog-based logging behind a `LoggerInterface`, keeping components decoupled from the logging backend |
| **Translation Support** | `TranslatorInterface` integrates Qt's `QTranslator` with a hook for loading additional translation files |
| **Qt Widgets** | Ready-made `AideTreeView`, `AideTableView`, `SearchFilterWidget`, and `SearchLineEdit` with placeholder text and tag highlighting |

---

## Getting started

New to aIDE? Start here:

1. [Overview](getting-started/overview.md) — understand the architecture and key concepts
2. [Installation](getting-started/installation.md) — integrate aIDE into your CMake project
3. [Quick Start](getting-started/quick-start.md) — build your first aIDE application in minutes

---

## Requirements

| Dependency | Minimum version |
|---|---|
| CMake | 3.25 |
| C++ compiler | C++20 (GCC 10+, Clang 12+, MSVC 2019 16.11+) |
| Qt | 5.15 LTS or 6.5+ |
| Conan | 2.x *(recommended)* |

**Platforms:** Linux, macOS, Windows.
