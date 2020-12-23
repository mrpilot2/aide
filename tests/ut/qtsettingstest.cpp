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
        auto group = HierarchicalId("Appearance")("Font");
        const auto fontSize{10};

        settings.setValue(group, "Size", fontSize);

        REQUIRE(settings.value(group, "Size") == fontSize);
    }

    SECTION("returns default value if key does not exist and default given")
    {
        auto group = HierarchicalId("Appearance")("Font");
        const auto* const fontName{"Arial"};

        auto val = settings.value(group, "Name", fontName);

        REQUIRE(val.toString().toStdString() == "Arial");
    }

    SECTION("returns empty variant if key does not exist and no default given")
    {
        auto group = HierarchicalId("Appearance")("Font");

        auto val = settings.value(group, "Name");

        REQUIRE(val == QVariant());
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
        auto group = HierarchicalId("Geometry")("MainWindow");
        const auto* geom{"a;dfjads;fjkads;fjkads;flkj"};

        settings.setValue(group, "Position", geom);

        REQUIRE(settings.value(group, "Position") == geom);

        QSettings s(aide::Application::organizationName(),
                    aide::Application::applicationName() + "_unversionable");
        auto fileName = s.fileName().toStdString();

        settings.load();
    }
}
