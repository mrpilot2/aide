# Notifications

aIDE's notification system covers four surfaces sharing one severity model (`NotificationType`) and one action model (`NotificationAction`):

| Surface | Logged? | Driven by | Auto-dismiss |
|---|---|---|---|
| **Balloon** | yes | `NotificationManager::post()` | transient balloons only |
| **Notification log** (`NotificationView`) | — | reads the manager's history | never |
| **Banner** | no | caller, direct widget use | caller-controlled |
| **"Got it" tooltip** (`GotItTooltip`) | no | caller, direct widget use | on click / timeout |

Only the balloon path goes through `NotificationManagerInterface::post()` and is recorded in the log. Banners and "Got it" tooltips are caller-driven, ephemeral, and never logged.

**Headers:** `aide/notificationmanagerinterface.hpp`, `aide/notification.hpp`, `aide/notificationgroup.hpp`, `aide/gui/widgets/notificationballoon.hpp`, `aide/gui/widgets/banner.hpp`, `aide/gui/widgets/gotittooltip.hpp`, `aide/gui/widgets/notificationview.hpp`

---

## NotificationManagerInterface

Reachable via `app.notificationManager()` (or `ApplicationBuilder::notificationManager()`). Decouples posting/querying code from the concrete `NotificationManager`.

### Groups

Every notification belongs to a `NotificationGroup` — the unit configured on the settings page and used for routing:

```cpp
aide::NotificationGroup group;
group.id                 = aide::HierarchicalId("myapp.build");
group.displayName         = "Build";
group.defaultDisplayType = aide::NotificationDisplayType::Balloon;
group.logByDefault        = true;

app.notificationManager().registerGroup(group);
```

Register groups once, at startup, before posting to them.

### Posting

```cpp
aide::Notification notification;
notification.groupId = aide::HierarchicalId("myapp.build");
notification.type    = aide::NotificationType::Success;
notification.title   = "Build finished";
notification.content = "0 errors, 3 warnings";
notification.actions.push_back({"Open log", [] { /* ... */ }});

const auto id = app.notificationManager().post(std::move(notification));
```

`post()` assigns `id` and `timestamp`, resolves and freezes the display type (see below), and appends the notification to the log. A `Notification` is an immutable snapshot once posted — it carries no read/dismissed flag; that lifecycle state lives in the manager.

### Display type resolution

`resolvedDisplayType(id)` is computed once, at `post()` time, and frozen — later settings changes never retroactively change how an already-posted notification displayed:

1. If Do Not Disturb is enabled, resolves to `None` (log-only).
2. Otherwise, a per-group settings override (`notifications.<group-id>.displayType`), if the user set one.
3. Otherwise, the group's `defaultDisplayType`.
4. `None` if the group was never registered.

`NotificationDisplayType` values: `None`, `Balloon`, `StickyBalloon`. These are group-routed popup destinations only — editor banners, dialog banners and "Got it" tooltips are caller-driven surfaces and deliberately have no enumerator here.

### Querying and lifecycle

```cpp
auto& mgr = app.notificationManager();

mgr.notifications();          // full log, for NotificationView
mgr.notification(id);         // std::optional<Notification>
mgr.isRead(id);
mgr.markRead(id);              // per-notification
mgr.markRead();                // global: clears hasUnread()
mgr.hasUnread();               // boolean, NOT a count — bind to an opener button's unread dot
mgr.remove(id);
mgr.clearAll();

mgr.setDoNotDisturb(true);
mgr.doNotDisturb();
```

`hasUnread()` is intentionally boolean rather than a count (per an amendment to the original design): `NotificationView` clears it via `markRead()` whenever it becomes visible, and again on every post that arrives while still visible.

### Signals (concrete `NotificationManager`)

`NotificationManagerInterface` itself declares no signals — connect to the concrete `aide::NotificationManager` (as `ApplicationBuilder`/`Application` already do internally for the balloon host):

```cpp
notificationPosted(aide::NotificationId id);
notificationUpdated(aide::NotificationId id);
notificationRemoved(aide::NotificationId id);
notificationsCleared();
unreadChanged(bool hasUnread);
```

---

## Balloons

`NotificationBalloonHost` (wired automatically by `ApplicationBuilder`) listens for `notificationPosted` and shows a `NotificationBalloon` for every notification whose frozen `resolvedDisplayType` is `Balloon` or `StickyBalloon`; `None` is log-only and never reaches this surface.

- Transient balloons (`Balloon`) auto-dismiss after a fixed delay with a countdown progress bar that pauses on hover.
- Sticky balloons (`StickyBalloon`) never time out — the user closes them explicitly.
- Corner placement is one global setting (`notifications.balloonPlacement`, a `NotificationBalloonPlacement`: `BottomRight`, `BottomLeft`, `TopRight`, `TopLeft`). Stacking direction derives from that corner alone — balloons anchor there and grow toward screen center as more stack up.

You never construct `NotificationBalloon` directly — it's created and owned exclusively by `NotificationBalloonHost`.

---

## Notification log view

`aide::widgets::NotificationView` is a plain embeddable `QWidget` showing the notification log — a flat, newest-first timeline (JetBrains Notifications-style), **not** a docked tool window. It is in-memory and session-only: nothing is persisted, and the timeline starts empty on every launch.

```cpp
auto* view = new aide::widgets::NotificationView(
    app.notificationManager(), *app.settingsProvider(), parent);

connect(view, &aide::widgets::NotificationView::settingsRequested, ...);      // "Notification Settings..." entry
connect(view, &aide::widgets::NotificationView::groupSettingsRequested, ...); // jump to a group's settings row
```

Bind an unread indicator on whatever opener button toggles this view to `NotificationManagerInterface::hasUnread()`; cards render uniform, with no per-entry read/unread styling.

---

## Banners

`aide::widgets::Banner` is a severity-tinted inline strip (coloured left stripe, type icon, message, optional action links, close button) for embedding directly in a window — e.g. a dialog banner or an editor banner. It reuses `NotificationType`/`NotificationAction` but never goes through `post()` and is never logged.

```cpp
auto* banner = new aide::widgets::Banner(
    aide::NotificationType::Warning, "Unsaved changes will be lost", parent);
banner->addAction({"Save", [] { /* ... */ }});
banner->setClosable(true);
connect(banner, &aide::widgets::Banner::closed, banner, &QObject::deleteLater);
```

Use banners for state tied to a specific window or editor, not for events that belong in the global notification log.

---

## "Got it" tooltip

`aide::widgets::GotItTooltip` is a one-shot onboarding tooltip anchored to a target widget, arrow pointing at it. Caller-driven: it never posts and never reaches the log, and depends only on `SettingsInterface` and the target widget — no `NotificationManager` dependency.

```cpp
auto* tip = new aide::widgets::GotItTooltip(
    *app.settingsProvider(), "myapp.newFeatureButton", "Click here to try the new feature", parent);
tip->withHeader("New!")
   .withLink("Learn more", [] { /* ... */ });

tip->showGotIt(targetButton, aide::widgets::GotItPosition::Below);
```

- Seen state is an integer show-count stored at settings key `aide/gotit/<id>` (`canShow() == count < maxCount`, default `maxCount` of 1). `withShowCount()` raises the limit.
- To reshow a changed tip, bump `id` (the `.v2` convention) — a fresh key starts its count at 0.
- Pass `id` as a string literal: it is threaded through `HierarchicalId`, which stores a raw pointer rather than copying.
- Only one tooltip shows at a time; closing one advances a single process-wide queue.
- `showGotIt()` is a no-op (and deletes the tooltip) if `canShow()` is false or the target is null.

`GotItTooltip::isFirstRunAfterUpgrade(settings)` is a separate static helper: it compares the stored previous-run version against `QCoreApplication::applicationVersion()`, records the current version, and reports whether this run follows an upgrade — useful for gating "what's new" tooltips.

---

## Settings page

`aide::gui::NotificationsSettingsPage` is the built-in settings page (registered automatically by `ApplicationBuilder`): Do Not Disturb + balloon placement controls above a flat table with one row per registered `NotificationGroup`, letting users override each group's display type.

Settings keys (see `aide/core/settings/notifications/notificationsettingskeys.hpp`):

| Key | Meaning |
|---|---|
| `notifications.doNotDisturb` | global suppress-all toggle |
| `notifications.balloonPlacement` | global corner anchor |
| `notifications.<group-id>.displayType` | per-group override of `defaultDisplayType` |

---

## Demo reference

`demo/src/notificationlauncherdialog.{hpp,cpp}` (opened via the demo's "Demo → Notifications" menu action) exercises every surface end-to-end: balloon/sticky-balloon posting with and without actions, the "Got it" tooltip with position controls, dialog banners, and editor banners. Use it as a live reference for wiring each surface.
