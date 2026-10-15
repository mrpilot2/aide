#ifndef AIDE_SPY_CHANGE_PAGE_CONTROLLER_HPP
#define AIDE_SPY_CHANGE_PAGE_CONTROLLER_HPP

#include <QItemSelection>
#include <QString>

#include "settings/settingsdialogchangepagecontroller.hpp"

namespace aide::test
{
    // Records which SettingsDialogChangePageController entry points the
    // settings dialog GUI layer forwarded to.
    class SpyChangePageController
        : public aide::core::SettingsDialogChangePageController
    {
    public:
        bool changeSelectedPageCalled{false};
        bool anyGuiElementChangedCalled{false};
        bool resetCurrentPageCalled{false};
        bool applyModifiedPagesCalled{false};
        bool searchPatternChangedCalled{false};
        bool commitCurrentSearchPatternCalled{false};
        bool showHelpForCurrentPageCalled{false};
        QString lastSearchPattern;

        void changeSelectedPage(const QItemSelection& /*selected*/,
                                const QItemSelection& /*deselected*/) override
        {
            changeSelectedPageCalled = true;
        }

        void searchPatternChanged(const QString& pattern) override
        {
            searchPatternChangedCalled = true;
            lastSearchPattern          = pattern;
        }

        void commitCurrentSearchPattern() override
        {
            commitCurrentSearchPatternCalled = true;
        }

        void anyGuiElementHasChanged() override
        {
            anyGuiElementChangedCalled = true;
        }

        void resetCurrentPage() override { resetCurrentPageCalled = true; }

        void applyModifiedSettingsPages() override
        {
            applyModifiedPagesCalled = true;
        }

        void showHelpForCurrentPage() override
        {
            showHelpForCurrentPageCalled = true;
        }
    };
} // namespace aide::test

#endif // AIDE_SPY_CHANGE_PAGE_CONTROLLER_HPP
