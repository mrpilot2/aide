#include <catch2/catch_test_macros.hpp>

#include <QApplication>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>

#include <aide/notificationtype.hpp>

#include "aide/gui/widgets/aidedialog.hpp"
#include "aide/gui/widgets/banner.hpp"

using aide::NotificationType;
using aide::widgets::AideDialog;
using aide::widgets::Banner;

namespace
{
    class TestDialog : public AideDialog
    {
    public:
        explicit TestDialog(QWidget* parent = nullptr)
            : AideDialog(parent)
        {
            contentLayout()->addWidget(new QLabel("Content", this));
        }
    };
} // namespace

TEST_CASE("An AideDialog's banner slot", "[AideDialog]")
{
    QApplication::setApplicationName("aide_test");
    QApplication::setOrganizationName("aide_company");

    SECTION("showBanner returns a banner mounted in the dialog")
    {
        AideDialog dialog;

        const auto* banner =
            dialog.showBanner(NotificationType::Warning, "Disk space low");

        REQUIRE(banner != nullptr);
        REQUIRE(dialog.isAncestorOf(banner));
    }

    SECTION("the banner sits above the content added via contentLayout()")
    {
        TestDialog dialog;

        const auto* banner =
            dialog.showBanner(NotificationType::Information, "Heads up");
        dialog.show();
        QApplication::processEvents();

        const auto* content = dialog.findChild<QLabel*>();
        REQUIRE(content != nullptr);
        REQUIRE(banner->y() < content->y());
    }

    SECTION("showBanner called again replaces the previous banner")
    {
        AideDialog dialog;

        const auto* first =
            dialog.showBanner(NotificationType::Information, "First");
        const auto* second =
            dialog.showBanner(NotificationType::Warning, "Second");
        QApplication::processEvents();

        REQUIRE(first != second);
        REQUIRE(dialog.findChildren<Banner*>().size() == 1);
    }

    SECTION("clearBanner removes the banner from the dialog")
    {
        AideDialog dialog;

        dialog.showBanner(NotificationType::Error, "Boom");
        dialog.clearBanner();
        QApplication::processEvents();

        REQUIRE(dialog.findChildren<Banner*>().isEmpty());
    }

    SECTION("clearBanner is a no-op when no banner is shown")
    {
        AideDialog dialog;

        dialog.clearBanner();

        REQUIRE(dialog.findChildren<Banner*>().isEmpty());
    }

    SECTION("closing a banner via its close button removes it from the dialog")
    {
        AideDialog dialog;

        dialog.showBanner(NotificationType::Success, "Done");
        auto* closeButton = dialog.findChild<QToolButton*>();
        REQUIRE(closeButton != nullptr);
        closeButton->click();
        QApplication::processEvents();

        REQUIRE(dialog.findChildren<Banner*>().isEmpty());
    }
}
