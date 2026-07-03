#include <array>
#include <memory>

#include <catch2/catch_test_macros.hpp>

#include <QAbstractButton>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QDoubleSpinBox>
#include <QItemSelection>
#include <QKeySequenceEdit>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QStandardItemModel>
#include <QTextEdit>
#include <QTreeView>
#include <QWidget>

#include "settings/changedetector.hpp"
#include "settings/settingsdialogchangepagecontroller.hpp"
#include "settings/settingsdialogcontroller.hpp"

namespace
{
    constexpr int kSpinValue{5};
    constexpr double kDoubleSpinValue{3.14};
    constexpr int kSliderMax{100};
    constexpr int kSliderValue{50};
    constexpr int kDateYear{2025};

    class SpyChangePageController
        : public aide::core::SettingsDialogChangePageController
    {
    public:
        bool anyGuiElementChangedCalled{false};

        void changeSelectedPage(const QItemSelection& /*selected*/,
                                const QItemSelection& /*deselected*/) override
        {}

        void anyGuiElementHasChanged() override
        {
            anyGuiElementChangedCalled = true;
        }

        void resetCurrentPage() override {}

        void applyModifiedSettingsPages() override {}
    };
} // namespace

TEST_CASE("installChangeDetector for QLineEdit", "[ChangeDetector]")
{
    SpyChangePageController spy;
    auto controller =
        std::make_shared<aide::gui::SettingsDialogController>(spy);

    SECTION("text change triggers onUserChangedAGuiElement")
    {
        QWidget container;
        auto* edit = new QLineEdit(&container);
        aide::gui::installChangeDetector(&container, controller);

        edit->setText("hello");

        REQUIRE(spy.anyGuiElementChangedCalled);
    }
}

TEST_CASE("installChangeDetector for QTextEdit", "[ChangeDetector]")
{
    SpyChangePageController spy;
    auto controller =
        std::make_shared<aide::gui::SettingsDialogController>(spy);

    SECTION("text change triggers onUserChangedAGuiElement")
    {
        QWidget container;
        auto* edit = new QTextEdit(&container);
        aide::gui::installChangeDetector(&container, controller);

        edit->setPlainText("hello");

        REQUIRE(spy.anyGuiElementChangedCalled);
    }
}

TEST_CASE("installChangeDetector for QPlainTextEdit", "[ChangeDetector]")
{
    SpyChangePageController spy;
    auto controller =
        std::make_shared<aide::gui::SettingsDialogController>(spy);

    SECTION("text change triggers onUserChangedAGuiElement")
    {
        QWidget container;
        auto* edit = new QPlainTextEdit(&container);
        aide::gui::installChangeDetector(&container, controller);

        edit->setPlainText("hello");

        REQUIRE(spy.anyGuiElementChangedCalled);
    }
}

TEST_CASE("installChangeDetector for QPushButton", "[ChangeDetector]")
{
    SpyChangePageController spy;
    auto controller =
        std::make_shared<aide::gui::SettingsDialogController>(spy);

    SECTION("click triggers onUserChangedAGuiElement")
    {
        QWidget container;
        auto* button = new QPushButton(&container);
        aide::gui::installChangeDetector(&container, controller);

        button->click();

        REQUIRE(spy.anyGuiElementChangedCalled);
    }
}

TEST_CASE("installChangeDetector for QCheckBox", "[ChangeDetector]")
{
    SpyChangePageController spy;
    auto controller =
        std::make_shared<aide::gui::SettingsDialogController>(spy);

    SECTION("click triggers onUserChangedAGuiElement")
    {
        QWidget container;
        auto* check = new QCheckBox(&container);
        aide::gui::installChangeDetector(&container, controller);

        check->click();

        REQUIRE(spy.anyGuiElementChangedCalled);
    }
}

TEST_CASE("installChangeDetector for QSpinBox", "[ChangeDetector]")
{
    SpyChangePageController spy;
    auto controller =
        std::make_shared<aide::gui::SettingsDialogController>(spy);

    SECTION("value change triggers onUserChangedAGuiElement")
    {
        QWidget container;
        auto* spin = new QSpinBox(&container);
        spin->setValue(0);
        aide::gui::installChangeDetector(&container, controller);

        spin->setValue(kSpinValue);

        REQUIRE(spy.anyGuiElementChangedCalled);
    }
}

TEST_CASE("installChangeDetector for QDoubleSpinBox", "[ChangeDetector]")
{
    SpyChangePageController spy;
    auto controller =
        std::make_shared<aide::gui::SettingsDialogController>(spy);

    SECTION("value change triggers onUserChangedAGuiElement")
    {
        QWidget container;
        auto* spin = new QDoubleSpinBox(&container);
        spin->setValue(0.0);
        aide::gui::installChangeDetector(&container, controller);

        spin->setValue(kDoubleSpinValue);

        REQUIRE(spy.anyGuiElementChangedCalled);
    }
}

TEST_CASE("installChangeDetector for QComboBox", "[ChangeDetector]")
{
    SpyChangePageController spy;
    auto controller =
        std::make_shared<aide::gui::SettingsDialogController>(spy);

    SECTION("index change triggers onUserChangedAGuiElement")
    {
        QWidget container;
        auto* combo = new QComboBox(&container);
        combo->addItem("one");
        combo->addItem("two");
        combo->setCurrentIndex(0);
        aide::gui::installChangeDetector(&container, controller);

        combo->setCurrentIndex(1);

        REQUIRE(spy.anyGuiElementChangedCalled);
    }
}

TEST_CASE("installChangeDetector for QSlider", "[ChangeDetector]")
{
    SpyChangePageController spy;
    auto controller =
        std::make_shared<aide::gui::SettingsDialogController>(spy);

    SECTION("value change triggers onUserChangedAGuiElement")
    {
        QWidget container;
        auto* slider = new QSlider(&container);
        slider->setRange(0, kSliderMax);
        slider->setValue(0);
        aide::gui::installChangeDetector(&container, controller);

        slider->setValue(kSliderValue);

        REQUIRE(spy.anyGuiElementChangedCalled);
    }
}

TEST_CASE("installChangeDetector for QDateTimeEdit", "[ChangeDetector]")
{
    SpyChangePageController spy;
    auto controller =
        std::make_shared<aide::gui::SettingsDialogController>(spy);

    SECTION("date change triggers onUserChangedAGuiElement")
    {
        QWidget container;
        auto* dte = new QDateTimeEdit(&container);
        aide::gui::installChangeDetector(&container, controller);

        dte->setDate(QDate(kDateYear, 1, 1));

        REQUIRE(spy.anyGuiElementChangedCalled);
    }
}

TEST_CASE("installChangeDetector for QKeySequenceEdit", "[ChangeDetector]")
{
    SpyChangePageController spy;
    auto controller =
        std::make_shared<aide::gui::SettingsDialogController>(spy);

    SECTION("key sequence change triggers onUserChangedAGuiElement")
    {
        QWidget container;
        auto* keyEdit = new QKeySequenceEdit(&container);
        aide::gui::installChangeDetector(&container, controller);

        keyEdit->setKeySequence(QKeySequence("Ctrl+A"));

        REQUIRE(spy.anyGuiElementChangedCalled);
    }
}

TEST_CASE("installChangeDetector for QAbstractItemView with model",
          "[ChangeDetector]")
{
    SpyChangePageController spy;
    auto controller =
        std::make_shared<aide::gui::SettingsDialogController>(spy);

    SECTION("model dataChanged triggers onUserChangedAGuiElement")
    {
        QWidget container;
        auto* view  = new QTreeView(&container);
        auto* model = new QStandardItemModel(1, 1, view);
        model->setItem(0, 0, new QStandardItem("original"));
        view->setModel(model);
        aide::gui::installChangeDetector(&container, controller);

        model->setData(model->index(0, 0), "changed", Qt::DisplayRole);

        REQUIRE(spy.anyGuiElementChangedCalled);
    }
}

TEST_CASE("installChangeDetector recurses into nested widgets",
          "[ChangeDetector]")
{
    SpyChangePageController spy;
    auto controller =
        std::make_shared<aide::gui::SettingsDialogController>(spy);

    SECTION("change in grandchild widget triggers onUserChangedAGuiElement")
    {
        QWidget outer;
        auto* inner = new QWidget(&outer);
        auto* edit  = new QLineEdit(inner);
        aide::gui::installChangeDetector(&outer, controller);

        edit->setText("nested");

        REQUIRE(spy.anyGuiElementChangedCalled);
    }
}

TEST_CASE("unInstallChangeDetector", "[ChangeDetector]")
{
    SpyChangePageController spy;
    auto controller =
        std::make_shared<aide::gui::SettingsDialogController>(spy);

    SECTION("after uninstall, widget change no longer triggers the controller")
    {
        QWidget container;
        auto* edit = new QLineEdit(&container);
        aide::gui::installChangeDetector(&container, controller);
        aide::gui::unInstallChangeDetector(&container, controller);

        edit->setText("should not trigger");

        REQUIRE(!spy.anyGuiElementChangedCalled);
    }
}
