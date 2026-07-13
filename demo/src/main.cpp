#include <QAction>
#include <QDir>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPalette>
#include <QString>

#include <aide/aideconstants.hpp>
#include <aide/aidesettingsprovider.hpp>
#include <aide/application.hpp>
#include <aide/colorscheme.hpp>
#include <aide/gui/translatorinterface.hpp>
#include <aide/menucontainerinterface.hpp>
#include <aide/theme.hpp>

#include "colorschemereactor.hpp"
#include "demosettingspage.hpp"

using aide::constants::CONSTANTS;

int main(int argc, char* argv[])
{
    aide::Application::setOrganizationName("aide_company");
    aide::Application::setApplicationName("Fancy_Aide_Demo");
    aide::Application::setApplicationDisplayName("Fancy Aide Demo");

    // The demo keeps every built-in feature enabled and passes the config
    // explicitly to show the API. A consumer that did not want the
    // View -> Full Screen action would disable it here, e.g.:
    //   config.setEnabled(
    //       aide::ApplicationConfig::Feature::ViewFullscreenAction, false);
    const aide::ApplicationConfig config;
    const aide::Application app(argc, argv, config);

    app.translator()->addAdditionalTranslationFilePath(
        QDir(":/demo_translations"), QString("demo"));

    // Register a consumer-defined theme
    constexpr int blueWindowR{220};
    constexpr int blueWindowG{235};
    constexpr int blueWindowB{255};
    constexpr int blueButtonR{200};
    constexpr int blueButtonG{220};
    constexpr int blueButtonB{255};
    QPalette bluePalette;
    bluePalette.setColor(QPalette::Window,
                         QColor(blueWindowR, blueWindowG, blueWindowB));
    bluePalette.setColor(QPalette::WindowText, Qt::black);
    bluePalette.setColor(QPalette::Base, Qt::white);
    // Text roles must be set explicitly: unset roles fall back to the system
    // default palette, which is light on a dark-default desktop and leaves
    // line edits and item views (tree/table) unreadable on the white Base.
    bluePalette.setColor(QPalette::Text, Qt::black);
    bluePalette.setColor(QPalette::AlternateBase,
                         QColor(blueWindowR, blueWindowG, blueWindowB));
    bluePalette.setColor(QPalette::ToolTipBase, Qt::white);
    bluePalette.setColor(QPalette::ToolTipText, Qt::black);
    bluePalette.setColor(QPalette::HighlightedText, Qt::white);
    bluePalette.setColor(QPalette::Button,
                         QColor(blueButtonR, blueButtonG, blueButtonB));
    bluePalette.setColor(QPalette::ButtonText, Qt::black);
    // Demo Blue ships its own icon set (white glyphs on a blue badge) so it is
    // easily told apart from aIDE's built-in aide-dark / aide-light icons. The
    // theme name must match the directory under the search path (see
    // demo_icons.qrc: :/demo/icons/demo-blue/index.theme).
    app.appearanceManager().registerTheme(
        {.name            = "Demo Blue",
         .palette         = bluePalette,
         .iconThemeName   = "demo-blue",
         .iconSearchPaths = {":/demo/icons"}});

    // Add icon search path for the System theme based on current color scheme
    app.appearanceManager().addIconSearchPath(
        "System",
        demo::iconPathForScheme(app.appearanceManager().colorScheme()));

    // Update demo icon search path when the OS color scheme changes. A
    // string-based connection to a QObject slot is used on purpose; see
    // demo::ColorSchemeReactor for the rationale.
    demo::ColorSchemeReactor colorSchemeReactor(app.appearanceManager());
    QObject::connect(
        &app.appearanceManager(), SIGNAL(colorSchemeChanged(aide::ColorScheme)),
        &colorSchemeReactor, SLOT(onColorSchemeChanged(aide::ColorScheme)));

    auto mainWindow = app.mainWindow();

    auto* description(new QLabel(
        QApplication::translate("",
                                "This text uses the translation system of aIDE "
                                "library within the demo application"),
        mainWindow.get()));

    mainWindow->setCentralWidget(description);

    app.settingsPageRegistry().addPage(std::make_unique<DemoSettingsPage>(
        aide::HierarchicalId("Demo Page 1")("Demo Subpage 1")(
            "Demo Subpage 1.1"),
        aide::AideSettingsProvider::unversionableSettings()));
    app.settingsPageRegistry().addPage(std::make_unique<DemoSettingsPage>(
        aide::HierarchicalId("Demo Page 1")("Demo Subpage 1")(
            "Demo Subpage 1.2"),
        aide::AideSettingsProvider::unversionableSettings()));

    // extend file menu

    auto actionRegistry{app.actionRegistry()};
    auto menuFileContainer{actionRegistry->getMenuContainer(
        aide::HierarchicalId("Main Menu")("File"))};
    auto actionNewProject =
        std::make_shared<QAction>(QApplication::tr("New project ..."));

    if (menuFileContainer.has_value()) {
        auto* menuFile{menuFileContainer.value()->menu()};
        actionNewProject->setParent(menuFile);
        QObject::connect(
            actionNewProject.get(), &QAction::triggered, [mainWindow]() {
                QMessageBox::information(
                    mainWindow.get(), QApplication::tr("Menu Extension"),
                    QApplication::tr("This action shows the menu extension "
                                     "capabilities of aIDE."));
            });
        // clang-format off
        menuFile->insertAction(
            actionRegistry->action(CONSTANTS().FILE_SETTINGS).value(), /* NOLINT(bugprone-unchecked-optional-access) */
            actionNewProject.get());
        menuFile->insertSeparator(
            actionRegistry->action(CONSTANTS().FILE_SETTINGS).value()); /* NOLINT(bugprone-unchecked-optional-access) */
        // clang-format on
        mainWindow->menuBar()->update();

        actionRegistry->registerAction(
            actionNewProject, aide::HierarchicalId("Main Menu")("File")("New"));
    }
    return aide::Application::exec();
}
