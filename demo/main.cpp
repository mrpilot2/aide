
#include <aide/application.hpp>

int main(int argc, char* argv[])
{
    aide::Application application(argc, argv);

    QMainWindow* mainWindow = application.mainWindow();

    mainWindow->show();

    return aide::Application::exec();
}
