# aIDE

A C++/Qt framework library that client applications build on to get IDE-style infrastructure: actions, settings, key bindings, logging, help.

## Language

### Help

**Help Topic**:
A unit of end-user help tied to a part of the UI, such as a settings page.
_Avoid_: Help page (for the identifier), help entry, help section

**Help Topic ID**:
The identifier of a Help Topic; the last part of the topic's URL relative to the Help Base URL (e.g. `keymap`).
_Avoid_: Help slug, help path, help key

**Help Base URL**:
The root URL of a client application's web help, registered by the client; a Help Topic resolves to this base plus its Help Topic ID.
_Avoid_: Help root, docs URL

**Help Page**:
The rendered web page a resolved Help Topic URL shows in the browser.
_Avoid_: Help Topic (for the page itself)

**End-User Help**:
Help content written for the users of a client application, distinct from the Developer Guide.
_Avoid_: User docs, manual

**Developer Guide**:
Documentation for developers building client applications on aIDE.
_Avoid_: Docs, help
