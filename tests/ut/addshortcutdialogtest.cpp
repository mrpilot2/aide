#include <array>

#include <catch2/catch_test_macros.hpp>

#include <QApplication>
#include <QGroupBox>
#include <QKeySequenceEdit>
#include <QLabel>

#include "settings/keymap/addshortcutdialog.hpp"

using aide::gui::AddShortcutDialog;

TEST_CASE("AddShortcutDialog", "[AddShortcutDialog]")
{
    // NOLINTNEXTLINE
    std::array<char*, 1> appName{{const_cast<char*>("aide_test")}};
    int numberOfArgs{1};
    const QApplication app(numberOfArgs, appName.data());

    SECTION("can be constructed without parent")
    {
        const AddShortcutDialog dialog;
    }

    SECTION("duplicate assignment group box is hidden on construction")
    {
        const AddShortcutDialog dialog;
        const auto* groupBox =
            dialog.findChild<QGroupBox*>("duplicateAssingmentGroupBox");
        REQUIRE(groupBox != nullptr);
        REQUIRE(!groupBox->isVisible());
    }

    SECTION("setAssignedActionDescription sets the label text")
    {
        const AddShortcutDialog dialog;
        dialog.setAssignedActionDescription("Open File");
        const auto* label = dialog.findChild<QLabel*>("shortCutNameLabel");
        REQUIRE(label != nullptr);
        REQUIRE(label->text() == "Open File");
    }

    SECTION("userSelectedKeySequence returns empty sequence by default")
    {
        const AddShortcutDialog dialog;
        REQUIRE(dialog.userSelectedKeySequence().isEmpty());
    }

    SECTION("key sequence edit exists as child widget")
    {
        const AddShortcutDialog dialog;
        const auto* keyEdit =
            dialog.findChild<QKeySequenceEdit*>("keySequenceEdit");
        REQUIRE(keyEdit != nullptr);
    }
}
