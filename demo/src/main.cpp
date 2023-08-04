#include <QAction>
#include <QDir>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QString>

#include <aide/aidesettingsprovider.hpp>
#include <aide/application.hpp>
#include <aide/gui/translatorinterface.hpp>
#include <aide/menucontainerinterface.hpp>
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

    aide::core::SettingsPageRegistry::addPage(
        std::make_unique<DemoSettingsPage>(
            aide::HierarchicalId("Demo Page 1")("Demo Subpage 1")(
                "Demo Subpage 1.1"),
            app.settingsProvider()->unversionableSettings()));
    aide::core::SettingsPageRegistry::addPage(
        std::make_unique<DemoSettingsPage>(
            aide::HierarchicalId("Demo Page 1")("Demo Subpage 1")(
                "Demo Subpage 1.2"),
            app.settingsProvider()->unversionableSettings()));

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
        menuFile->insertAction(
            actionRegistry
                ->action(aide::HierarchicalId("Main Menu")("File")("Settings"))
                .value(),
            actionNewProject.get());
        menuFile->insertSeparator(
            actionRegistry
                ->action(aide::HierarchicalId("Main Menu")("File")("Settings"))
                .value());
        mainWindow->menuBar()->update();

        actionRegistry->registerAction(
            actionNewProject, aide::HierarchicalId("Main Menu")("File")("New"));
    }
    return aide::Application::exec();
}
