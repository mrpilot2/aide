
#ifndef AIDE_KEYMAP_PAGE_WIDGET_HPP
#define AIDE_KEYMAP_PAGE_WIDGET_HPP

#include <memory>

#include <QWidget>

namespace Ui
{
    class KeymapPageWidget;
} // namespace Ui

namespace aide::gui
{
    class KeymapPageWidget : public QWidget
    {
    public:
        explicit KeymapPageWidget(QWidget* parent = nullptr);
        ~KeymapPageWidget() override;
        KeymapPageWidget(const KeymapPageWidget&) = delete;
        KeymapPageWidget& operator=(const KeymapPageWidget&) = delete;
        KeymapPageWidget(KeymapPageWidget&&)                 = delete;
        KeymapPageWidget& operator=(KeymapPageWidget&&) = delete;

    private:
        std::unique_ptr<Ui::KeymapPageWidget> ui;
    };
} // namespace aide::gui

#endif // AIDE_KEYMAP_PAGE_WIDGET_HPP
