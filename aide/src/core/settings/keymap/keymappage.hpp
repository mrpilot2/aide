
#ifndef AIDE_KEYMAP_PAGE_HPP
#define AIDE_KEYMAP_PAGE_HPP

#include <settings/settingspage.hpp>

#include "../../../gui/settings/keymap/keymappagewidget.hpp"

namespace aide::core
{
    class KeymapPage : public SettingsPage
    {
    public:
        KeymapPage();

        QWidget* widget() override;
        [[nodiscard]] bool isModified() const override;
        void reset() override;
        void apply() override;

    private:
        aide::gui::KeymapPageWidget* m_widget;
    };
} // namespace aide::core

#endif // AIDE_KEYMAP_PAGE_HPP
