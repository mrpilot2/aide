#include <QDir>
#include <QLabel>
#include <QMainWindow>
#include <QString>

#include <aide/application.hpp>
#include <aide/gui/translatorinterface.hpp>

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

    return aide::Application::exec();
}
