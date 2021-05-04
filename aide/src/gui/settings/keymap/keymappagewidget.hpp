
#ifndef AIDE_KEYMAP_PAGE_WIDGET_HPP
#define AIDE_KEYMAP_PAGE_WIDGET_HPP

#include <memory>

#include <QWidget>

#include <settings/keymap/keymappagewidgetinterface.hpp>

namespace Ui
{
    class KeymapPageWidget;
} // namespace Ui

namespace aide::gui
{
    class KeymapPageWidget
        : public QWidget
        , public aide::core::KeyMapPageWidgetInterface

    {
    public:
        explicit KeymapPageWidget(QWidget* parent = nullptr);
        ~KeymapPageWidget() override;
        KeymapPageWidget(const KeymapPageWidget&) = delete;
        KeymapPageWidget& operator=(const KeymapPageWidget&) = delete;
        KeymapPageWidget(KeymapPageWidget&&)                 = delete;
        KeymapPageWidget& operator=(KeymapPageWidget&&) = delete;

        void setTreeModel(std::shared_ptr<QAbstractItemModel> model) override;

    private:
        std::unique_ptr<Ui::KeymapPageWidget> ui;
    };
} // namespace aide::gui

#endif // AIDE_KEYMAP_PAGE_WIDGET_HPP
