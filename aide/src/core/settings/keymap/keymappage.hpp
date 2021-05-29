
#ifndef AIDE_KEYMAP_PAGE_HPP
#define AIDE_KEYMAP_PAGE_HPP

#include <settings/settingspage.hpp>

#include "actionregistryinterface.hpp"
#include "showkeymap.hpp"

namespace aide::core
{
    class KeymapPage : public SettingsPage
    {
    public:
        explicit KeymapPage(ActionRegistryInterfacePtr registry,
                            KeyMapPageWidgetInterface* widget);

        QWidget* widget() override;
        [[nodiscard]] bool isModified() const override;
        void reset() override;
        void apply() override;

        KeyMapPageWidgetInterface* keyMapWidget();

    private:
        ActionRegistryInterfacePtr actionRegistry;

        KeyMapPageWidgetInterface* m_widget;

        ShowKeyMap showUseCase;
    };
} // namespace aide::core

#endif // AIDE_KEYMAP_PAGE_HPP
