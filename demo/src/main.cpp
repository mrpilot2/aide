#include <QDir>
#include <QString>

#include <aide/application.hpp>
#include <aide/gui/translatorinterface.hpp>

#include "colorschemereactor.hpp"
#include "demosetup.hpp"

int main(int argc, char* argv[])
{
    aide::Application::setOrganizationName("aide_company");
    aide::Application::setApplicationName("Fancy_Aide_Demo");
    aide::Application::setApplicationDisplayName("Fancy Aide Demo");

    // See demo::demoConfig() for the features, URL launcher and help the
    // demo configures.
    const aide::Application app(argc, argv, demo::demoConfig());

    app.translator()->addAdditionalTranslationFilePath(
        QDir(":/demo_translations"), QString("demo"));

    // cppcheck-suppress unreadVariable
    const auto colorSchemeReactor = demo::setUpDemo(app);

    return aide::Application::exec();
}
