
#include "catch.hpp"

#include <chrono>
#include <thread>

#include <aide/application.hpp>

TEST_CASE("A new application")
{
    SECTION(" creates a main window")
    {
        int argc{1};
        char* argv[] = {const_cast<char*>("a")};
        aide::Application app(argc, argv);

        REQUIRE(app.mainWindow() != nullptr);
    }
}

TEST_CASE("Any application ")
{
    SECTION(" blocks when executed until main window closes")
    {
        int argc{1};
        char* argv[] = {const_cast<char*>("a")};
        aide::Application app(argc, argv);

        QMainWindow* mainWindow = app.mainWindow();

        const auto start = std::chrono::steady_clock::now();
        const std::chrono::milliseconds sleepTime(10);
        std::thread t([mainWindow, sleepTime]() {
            std::this_thread::sleep_for(sleepTime);
            mainWindow->close();
        });

        aide::Application::exec();

        const auto end = std::chrono::steady_clock::now();
        const auto diff = end - start;

        REQUIRE(std::chrono::duration_cast<std::chrono::milliseconds>(diff)
                    .count() >= sleepTime.count());
        t.join();
    }

    SECTION(" when executed returns 0 if application finishes normally")
    {
        int argc{1};
        char* argv[] = {const_cast<char*>("a")};
        aide::Application app(argc, argv);

        const std::chrono::milliseconds sleepTime(10);
        std::thread t([sleepTime]() {
            std::this_thread::sleep_for(sleepTime);
            QApplication::quit();
        });

        REQUIRE(aide::Application::exec() == 0);
        t.join();
    }

    SECTION(" when executed returns the exit code of the application")
    {
        int argc{1};
        char* argv[] = {const_cast<char*>("a")};
        aide::Application app(argc, argv);

        int exitCode{-1};
        const std::chrono::milliseconds sleepTime(10);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
        std::thread t([sleepTime, exitCode]() {
            std::this_thread::sleep_for(sleepTime);
            QApplication::exit(exitCode);
        });

#pragma clang diagnostic pop

        REQUIRE(aide::Application::exec() == exitCode);
        t.join();
    }
}
