# aIDE

A C++/Qt framework library that client applications build on to get IDE-style infrastructure: actions, settings, key bindings, logging, help.

## Language

### Help

**Help Topic**:
A unit of end-user help tied to a part of the UI, such as a settings page.
_Avoid_: Help page (for the identifier), help entry, help section

**Help Topic ID**:
The identifier a subsystem declares for its Help Topic; a relative path that, unless the client remaps it, is the topic's location under the Help Base URL (e.g. `keymap`, `settings/keymap`).
_Avoid_: Help slug, help path, help key

**Help Base URL**:
The root URL of a client application's web help, registered by the client; a Help Topic resolves to this base plus its Help Topic ID.
_Avoid_: Help root, docs URL

**Help Page**:
The rendered web page a resolved Help Topic URL shows in the browser.
_Avoid_: Help Topic (for the page itself)

**Context Help**:
Opening the Help Topic of whatever the user is working in, found from the focused part of the UI outward; falls back to Help Contents when nothing there has a topic.
_Avoid_: F1 help (the key can be remapped), contextual help

**Help Contents**:
The entry point of a client's End-User Help, the page at the Help Base URL itself; not a Help Topic.
_Avoid_: Help index, help home

**End-User Help**:
Help content written for the users of a client application, distinct from the Developer Guide.
_Avoid_: User docs, manual

**Developer Guide**:
Documentation for developers building client applications on aIDE.
_Avoid_: Docs, help
