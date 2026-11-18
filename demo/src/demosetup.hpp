#ifndef DEMO_DEMO_SETUP_HPP
#define DEMO_DEMO_SETUP_HPP

#include <memory>

#include <QAction>

#include <aide/applicationconfig.hpp>
#include <aide/urllauncherinterface.hpp>

namespace aide
{
    class Application;
} // namespace aide

namespace demo
{
    class ColorSchemeReactor;

    /// The demo's ApplicationConfig: every built-in feature on, Report Bug
    /// previewed instead of filed, and help pointed at the demo End-User Help
    /// site (demo/help) on GitHub Pages. @p browser opens every other URL,
    /// e.g. Help Pages; nullptr means the system's default browser.
    [[nodiscard]] aide::ApplicationConfig demoConfig(
        aide::UrlLauncherPtr browser = nullptr);

    /// Builds the demo on top of @p app, which must have been constructed
    /// from demoConfig(): theme, central widget, settings pages and menus.
    /// The returned reactor must outlive the application event loop (see
    /// watchColorScheme()).
    [[nodiscard]] std::unique_ptr<ColorSchemeReactor> setUpDemo(
        const aide::Application& app);

    /// Registers the "Demo Blue" theme and points the "System" theme's icon
    /// search path at the icon set matching the current OS color scheme.
    void registerDemoTheme(const aide::Application& app);

    /// Keeps the "System" theme's icon search path in sync with the OS color
    /// scheme. The returned reactor must outlive the application event loop.
    [[nodiscard]] std::unique_ptr<ColorSchemeReactor> watchColorScheme(
        const aide::Application& app);

    /// Builds the central splitter (demo description + notification log) and
    /// installs it as the main window's central widget.
    void buildCentralWidget(const aide::Application& app);

    /// Registers the demo's example settings pages.
    void registerDemoSettingsPages(const aide::Application& app);

    /// Adds a "New project ..." entry to the File menu. The returned action
    /// is owned by the File menu; the caller does not need to keep it alive.
    QAction* extendFileMenu(const aide::Application& app);

    /// Adds a checkable "Notification Log" toggle to the View menu, wired to
    /// persist visibility and splitter state via the demo's settings. The
    /// returned action is owned by the View menu.
    QAction* addNotificationLogToggle(const aide::Application& app);

    /// Adds a "Demo" menu (before "Help") with a Notifications launcher
    /// action, and registers the demo notification groups it exercises. The
    /// returned action is owned by the Demo menu.
    QAction* buildDemoMenu(const aide::Application& app);
} // namespace demo

#endif // DEMO_DEMO_SETUP_HPP
