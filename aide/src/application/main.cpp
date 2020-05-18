//
// Created by markus on 18.05.20.
//

#include <aide/application.hpp>

int main(int argc, char* argv[])
{
    aide::Application::setApplicationName("aide_test2");
    aide::Application::setOrganizationName("aide_company");
    aide::Application app(argc, argv);
}