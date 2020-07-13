#ifndef AIDE_SEARCHLINEEDIT_HPP
#define AIDE_SEARCHLINEEDIT_HPP

#include <memory>

#include <QWidget>

namespace Ui
{
    class SearchLineEdit;
} // namespace Ui

namespace aide
{
    class SearchLineEdit : public QWidget
    {
    public:
        explicit SearchLineEdit(QWidget* parent);

        ~SearchLineEdit() override;

        void setSearchHint(const std::string& searchHint);

        void setSearchIcon(const QIcon& icon);

    private:
        std::unique_ptr<Ui::SearchLineEdit> m_ui;
    };
} // namespace aide

#endif // AIDE_SEARCHLINEEDIT_HPP
