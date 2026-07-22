#ifndef DEMO_DEMO_SETUP_HPP
#define DEMO_DEMO_SETUP_HPP

#include <memory>

#include <QAction>

namespace aide
{
    class Application;
} // namespace aide

namespace demo
{
    class ColorSchemeReactor;

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

    /// Adds a "New project ..." entry to the File menu.
    [[nodiscard]] std::shared_ptr<QAction> extendFileMenu(
        const aide::Application& app);

    /// Adds a checkable "Notification Log" toggle to the View menu, wired to
    /// persist visibility and splitter state via the demo's settings.
    [[nodiscard]] std::shared_ptr<QAction> addNotificationLogToggle(
        const aide::Application& app);

    /// Adds a "Demo" menu (before "Help") with a Notifications launcher
    /// action, and registers the demo notification groups it exercises.
    [[nodiscard]] std::shared_ptr<QAction> buildDemoMenu(
        const aide::Application& app);
} // namespace demo

#endif // DEMO_DEMO_SETUP_HPP
