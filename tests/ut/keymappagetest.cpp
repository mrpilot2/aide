#include <array>

#include <catch2/catch.hpp>

#include <QApplication>

#include "actionregistry.hpp"
#include "hierarchicalid.hpp"
#include "mockkeymappagewidget.hpp"
#include "mocksettings.hpp"
#include "nulllogger.hpp"
#include "settings/keymap/keymappage.hpp"

using aide::ActionRegistry;
using aide::HierarchicalId;
using aide::core::KeymapPage;
using aide::test::MockKeyMapPageWidget;
using aide::test::MockSettings;
using aide::test::NullLogger;

TEST_CASE("A new keymap page")
{
    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};

    QApplication app{numberOfArgs, appName.data()};

    MockSettings settings;
    auto logger = std::make_shared<NullLogger>();
    auto registry(std::make_shared<ActionRegistry>(settings, logger));

    auto widget = std::make_shared<MockKeyMapPageWidget>();

    KeymapPage page{registry, widget.get()};

    SECTION("returns a valid widget") { REQUIRE(page.widget() != nullptr); }

    SECTION("returns a valid keymap widget")
    {
        REQUIRE(page.keyMapWidget() != nullptr);
    }

    SECTION("is not modified by default")
    {
        REQUIRE(page.isModified() == false);
    }
}

TEST_CASE("Any keymap page")
{
    int numberOfArgs{1};
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};

    QApplication app{numberOfArgs, appName.data()};

    MockSettings settings;
    auto logger = std::make_shared<NullLogger>();
    auto registry(std::make_shared<ActionRegistry>(settings, logger));

    std::shared_ptr<QAction> action{std::make_shared<QAction>()};

    registry->registerAction(action,
                             HierarchicalId("Main Menu")("File")("New File"),
                             {QKeySequence("Alt+F4")});

    auto widget = std::make_unique<MockKeyMapPageWidget>();

    KeymapPage page{registry, widget.get()};

    SECTION("fills the tree model on reset")
    {
        page.reset();

        REQUIRE(widget->wasTreeModelSet());
    }

    SECTION("detects modification if the action registry and tree model differ")
    {
        auto model = page.getTreeModel();

        QModelIndex index(model->index(
            0, 1, model->index(0, 0, model->index(0, 0, QModelIndex()))));

        page.getTreeModel()->setData(index, "Alt+F5", Qt::DisplayRole);

        REQUIRE(page.isModified());
    }

    SECTION(
        "does not detects modification if the tree model is set back to the "
        "action registry")
    {
        auto model = page.getTreeModel();

        QModelIndex index(model->index(
            0, 1, model->index(0, 0, model->index(0, 0, QModelIndex()))));

        page.getTreeModel()->setData(index, "Alt+F5", Qt::DisplayRole);
        page.getTreeModel()->setData(index, "Alt+F4", Qt::DisplayRole);

        REQUIRE(!page.isModified());
    }

    SECTION("sets the shortcuts to the previous shortcut on reset")
    {
        auto model = page.getTreeModel();

        QModelIndex index(model->index(
            0, 1, model->index(0, 0, model->index(0, 0, QModelIndex()))));

        page.getTreeModel()->setData(index, "Alt+F5", Qt::DisplayRole);

        page.reset();

        REQUIRE(!page.isModified());
        REQUIRE(model->data(index, Qt::DisplayRole).toString().toStdString() ==
                "Alt+F4");
    }

    SECTION("applies modified shortcuts in the action registry")
    {
        auto model = page.getTreeModel();

        QModelIndex index(model->index(
            0, 1, model->index(0, 0, model->index(0, 0, QModelIndex()))));

        page.getTreeModel()->setData(index, "Alt+F5", Qt::DisplayRole);

        page.apply();

        REQUIRE(registry->actions()
                    .at(HierarchicalId("Main Menu")("File")("New File"))
                    .keySequences ==
                QList<QKeySequence>({QKeySequence::fromString("Alt+F5")}));
    }
}
