#include <memory>

#include <catch2/catch_test_macros.hpp>

#include "hierarchicalid.hpp"
#include "mocksettingspage.hpp"
#include "settings/settingspagefilterproxymodel.hpp"
#include "settings/settingspagegrouptreemodel.hpp"
#include "settings/settingspageregistry.hpp"

using aide::HierarchicalId;
using aide::core::SettingsPageFilterProxyModel;
using aide::core::SettingsPageGroupTreeModel;
using aide::core::SettingsPageRegistry;
using aide::test::MockSettingsPage;

namespace
{
    QString displayName(const QAbstractItemModel& model, int row,
                        const QModelIndex& parent = QModelIndex())
    {
        return model.data(model.index(row, 0, parent), Qt::DisplayRole)
            .toString();
    }
} // namespace

TEST_CASE("A settings page filter proxy model")
{
    SettingsPageRegistry registry;
    registry.addPage(
        std::make_unique<MockSettingsPage>(HierarchicalId("Appearance")));
    registry.addPage(
        std::make_unique<MockSettingsPage>(HierarchicalId("Editor")("Fonts")));
    registry.addPage(
        std::make_unique<MockSettingsPage>(HierarchicalId("Editor")("Colors")));

    auto treeModel = std::make_shared<SettingsPageGroupTreeModel>(registry);

    SettingsPageFilterProxyModel proxy;
    proxy.setSourceModel(treeModel.get());

    SECTION("shows the full tree when no pattern is set")
    {
        REQUIRE(proxy.rowCount() == 2);
    }

    SECTION("filters top-level pages by display name")
    {
        proxy.setSearchPattern("Appearance");

        REQUIRE(proxy.rowCount() == 1);
        REQUIRE(displayName(proxy, 0) == "Appearance");
    }

    SECTION("keeps parent nodes visible when a child matches")
    {
        proxy.setSearchPattern("Fonts");

        REQUIRE(proxy.rowCount() == 1);
        REQUIRE(displayName(proxy, 0) == "Editor");

        const auto editor = proxy.index(0, 0, QModelIndex());
        REQUIRE(proxy.rowCount(editor) == 1);
        REQUIRE(displayName(proxy, 0, editor) == "Fonts");
    }

    SECTION("matches case-insensitively")
    {
        proxy.setSearchPattern("fOnTs");

        REQUIRE(proxy.rowCount() == 1);
        const auto editor = proxy.index(0, 0, QModelIndex());
        REQUIRE(displayName(proxy, 0, editor) == "Fonts");
    }

    SECTION("hides everything when nothing matches")
    {
        proxy.setSearchPattern("does-not-exist");

        REQUIRE(proxy.rowCount() == 0);
    }

    SECTION("restores the full tree when the pattern is cleared")
    {
        proxy.setSearchPattern("Appearance");
        REQUIRE(proxy.rowCount() == 1);

        proxy.setSearchPattern("");

        REQUIRE(proxy.rowCount() == 2);
    }
}

TEST_CASE("A settings page filter proxy model filtering by page content")
{
    SettingsPageRegistry registry;
    registry.addPage(std::make_unique<MockSettingsPage>(
        HierarchicalId("Network"), "Proxy server address"));
    registry.addPage(
        std::make_unique<MockSettingsPage>(HierarchicalId("Appearance")));

    auto treeModel = std::make_shared<SettingsPageGroupTreeModel>(registry);

    SettingsPageFilterProxyModel proxy;
    proxy.setSourceModel(treeModel.get());

    SECTION("shows a page whose content matches even if the name does not")
    {
        proxy.setSearchPattern("server address");

        REQUIRE(proxy.rowCount() == 1);
        REQUIRE(displayName(proxy, 0) == "Network");
    }

    SECTION("hides pages when neither name nor content matches")
    {
        proxy.setSearchPattern("zzz qqq");

        REQUIRE(proxy.rowCount() == 0);
    }

    SECTION("shows a page whose content matches any one of the search words")
    {
        proxy.setSearchPattern("server nonexistent");

        REQUIRE(proxy.rowCount() == 1);
        REQUIRE(displayName(proxy, 0) == "Network");
    }

    SECTION("shows pages matched by different words on different pages")
    {
        proxy.setSearchPattern("appearance server");

        REQUIRE(proxy.rowCount() == 2);
    }
}

TEST_CASE("A settings page filter proxy model matching translated titles")
{
    SettingsPageRegistry registry;
    auto page =
        std::make_shared<MockSettingsPage>(HierarchicalId("Appearance"));
    page->setGroupTitles({"Erscheinungsbild"});
    registry.addPage(page);

    auto treeModel = std::make_shared<SettingsPageGroupTreeModel>(registry);

    SettingsPageFilterProxyModel proxy;
    proxy.setSourceModel(treeModel.get());

    SECTION("matches the translated display text rather than the raw id")
    {
        proxy.setSearchPattern("Erscheinungsbild");

        REQUIRE(proxy.rowCount() == 1);
        REQUIRE(displayName(proxy, 0) == "Erscheinungsbild");
    }

    SECTION("no longer matches the untranslated raw id")
    {
        proxy.setSearchPattern("Appearance");

        REQUIRE(proxy.rowCount() == 0);
    }
}
