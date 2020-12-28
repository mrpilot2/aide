#include <array>
#include <string>

#include <catch2/catch.hpp>

#include <QSettings>

#include "application.hpp"
#include "hierarchicalid.hpp"
#include "qtsettings.hpp"

using aide::Application;
using aide::HierarchicalId;
using aide::QtSettings;

TEST_CASE("Versionable Qt Settings")
{
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    int numberOfArgs{1};

    aide::Application::setApplicationName("aide_test");
    aide::Application::setOrganizationName("aide_company");

    aide::Application app(numberOfArgs, appName.data());

    auto settings = QtSettings(true);

    SECTION("write to default Qt Settings file")
    {
        auto group = HierarchicalId("Appearance")("Font")("Size");
        const auto fontSize{10};

        settings.setValue(group, fontSize);

        REQUIRE(settings.value(group) == fontSize);
    }

    SECTION("returns default value if key does not exist and default given")
    {
        auto group = HierarchicalId("Appearance")("Font")("Name");
        const auto* const fontName{"Arial"};

        auto val = settings.value(group, fontName);

        REQUIRE(val.toString().toStdString() == "Arial");
    }

    SECTION("returns empty variant if key does not exist and no default given")
    {
        auto group = HierarchicalId("Appearance")("Font")("Name");

        auto val = settings.value(group);

        REQUIRE(val == QVariant());
    }

    SECTION("does not segfault if only key is given")
    {
        auto key = HierarchicalId("Size");

        settings.setValue(key, "abc");
        auto val = settings.value(key);
        REQUIRE(val.toString().toStdString() == "abc");
    }
}

TEST_CASE("Un-Versionable Qt Settings")
{
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    int numberOfArgs{1};

    aide::Application::setApplicationName("aide_test");
    aide::Application::setOrganizationName("aide_company");

    aide::Application app(numberOfArgs, appName.data());

    auto settings = QtSettings(false);

    SECTION("write to default Qt Settings file with suffix _unversionable")
    {
        auto group = HierarchicalId("Geometry")("MainWindow")("Position");
        const auto* geom{"a;dfjads;fjkads;fjkads;flkj"};

        settings.setValue(group, geom);

        REQUIRE(settings.value(group, "Position") == geom);

        QSettings s(aide::Application::organizationName(),
                    aide::Application::applicationName() + "_unversionable");
        auto fileName = s.fileName().toStdString();

        settings.load();
    }
}
