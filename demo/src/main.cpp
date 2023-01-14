#include <QDir>
#include <QLabel>
#include <QMainWindow>
#include <QString>

#include <aide/application.hpp>
#include <aide/gui/translatorinterface.hpp>
#include <aide/settings/settingspageregistry.hpp>

#include "demosettingspage.hpp"

int main(int argc, char* argv[])
{
    aide::Application::setOrganizationName("aide_company");
    aide::Application::setApplicationName("Fancy_Aide_Demo");
    aide::Application::setApplicationDisplayName("Fancy Aide Demo");

    aide::Application app(argc, argv);

    app.translator()->addAdditionalTranslationFilePath(
        QDir(":/demo_translations"), QString("demo"));

    auto mainWindow = app.mainWindow();

    auto* description(new QLabel(
        QApplication::translate("",
                                "This text uses the translation system of aIDE "
                                "library within the demo application"),
        mainWindow.get()));

    mainWindow->setCentralWidget(description);

    //    aide::core::SettingsPageRegistry::addPage(
    //        std::make_unique<DemoSettingsPage>(
    //            aide::HierarchicalId("Demo Page 1")("Demo Subpage 1")(
    //                "Demo Subpage 1.1"),
    //            app.settingsProvider()->unversionableSettings()));
    //    aide::core::SettingsPageRegistry::addPage(
    //        std::make_unique<DemoSettingsPage>(
    //            aide::HierarchicalId("Demo Page 1")("Demo Subpage 1")(
    //                "Demo Subpage 1.2"),
    //            app.settingsProvider()->unversionableSettings()));
    //
    //    aide::core::SettingsPageRegistry::addPage(
    //        std::make_unique<DemoSettingsPage>(
    //            aide::HierarchicalId("Demo Page 1")("Demo Subpage 2")(
    //                "Demo Subpage 2.1"),
    //            app.settingsProvider()->unversionableSettings()));
    //
    //    aide::core::SettingsPageRegistry::addPage(
    //        std::make_unique<DemoSettingsPage>(
    //            aide::HierarchicalId("Demo Page 1")("Demo Subpage 2")(
    //                "Demo Subpage 2.2"),
    //            app.settingsProvider()->unversionableSettings()));
    //    aide::core::SettingsPageRegistry::addPage(
    //        std::make_unique<DemoSettingsPage>(
    //            aide::HierarchicalId("Demo Page 2"),
    //            app.settingsProvider()->unversionableSettings()));

    return aide::Application::exec();
}
