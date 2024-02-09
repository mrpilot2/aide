#include <array>

#include <catch2/catch.hpp>

#include <QApplication>

#include "applicationtranslator.hpp"
#include "nulllogger.hpp"

using aide::gui::ApplicationTranslator;
using aide::test::NullLogger;

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST_CASE("Default ApplicationTranslator")
{
    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    QApplication app(numberOfArgs, appName.data());

    SECTION(" finds at least two languages by default")
    {
        ApplicationTranslator translator(std::make_shared<NullLogger>());

        REQUIRE(translator.getAvailableTranslations().size() >= 2);
    }

    SECTION(" finds at least german and english by default")
    {
        ApplicationTranslator translator(std::make_shared<NullLogger>());

        const std::set availableTranslations{
            translator.getAvailableTranslations()};

        // On Ubuntu 20.04 United States is written with a space, on older
        // ubuntu version, there is no space
        REQUIRE((availableTranslations.find("English (United States)") !=
                     availableTranslations.end() ||
                 availableTranslations.find("English (UnitedStates)") !=
                     availableTranslations.end()));
        REQUIRE(availableTranslations.find("German (Germany)") !=
                availableTranslations.end());
    }

    SECTION(" allows to translate a standard library string")
    {
        QLocale::setDefault(QLocale(QLocale::German));
        ApplicationTranslator translator(std::make_shared<NullLogger>());

        REQUIRE(QApplication::translate("aide::gui::MainWindow", "Quit") ==
                QString("Beenden"));
    }
}

TEST_CASE("ApplicationTranslator with additional path")
{
    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    QApplication app(numberOfArgs, appName.data());

    SECTION(" finds only application languages")
    {
        ApplicationTranslator translator(std::make_shared<NullLogger>());

        translator.addAdditionalTranslationFilePath(QDir(":/ut_translations"),
                                                    QString("ut"));
        REQUIRE(translator.getAvailableTranslations().size() == 1);

        const std::set availableTranslations{
            translator.getAvailableTranslations()};
        REQUIRE(availableTranslations.find("Swedish (Sweden)") !=
                availableTranslations.end());
    }

    SECTION(" is able to translate to application language")
    {
        QLocale::setDefault(QLocale(QLocale::Swedish));
        ApplicationTranslator translator(std::make_shared<NullLogger>());

        translator.addAdditionalTranslationFilePath(QDir(":/ut_translations"),
                                                    QString("ut"));

        REQUIRE(QApplication::translate("Unit Test", "Translation") ==
                QString("Översättning"));
    }
}
