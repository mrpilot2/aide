#include <QDir>
#include <QString>

#include <aide/application.hpp>
#include <aide/gui/translatorinterface.hpp>

#include "colorschemereactor.hpp"
#include "demosetup.hpp"
#include "reportbugpreviewlauncher.hpp"

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
    //
    // Two exceptions: ShowLogInFileManagerAction and ReportBugAction default
    // to disabled since they are only appropriate for developer-facing
    // consumer applications. The demo opts in explicitly so both features
    // are visibly exercised by anyone running it.
    //
    // ReportBugAction also gets a demo-supplied UrlLauncherInterface
    // override, so clicking the action shows a preview dialog of the ticket
    // that would have been filed instead of opening a real browser and
    // filing a real issue against aIDE's own repository.
    aide::ApplicationConfig config;
    config.setEnabled(
        aide::ApplicationConfig::Feature::ShowLogInFileManagerAction, true);
    config.setEnabled(aide::ApplicationConfig::Feature::ReportBugAction, true);
    config.setUrlLauncher(std::make_shared<demo::ReportBugPreviewLauncher>());
    const aide::Application app(argc, argv, config);

    app.translator()->addAdditionalTranslationFilePath(
        QDir(":/demo_translations"), QString("demo"));

    demo::registerDemoTheme(app);
    auto colorSchemeReactor = demo::watchColorScheme(app);

    demo::buildCentralWidget(app);
    demo::registerDemoSettingsPages(app);

    // Actions returned below are kept alive for the whole event loop: see
    // the ownership note in demosetup.hpp.
    auto actionNewProject      = demo::extendFileMenu(app);
    auto actionNotificationLog = demo::addNotificationLogToggle(app);
    auto actionNotifications   = demo::buildDemoMenu(app);

    return aide::Application::exec();
}
