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
    namespace gui
    {
        class SearchLineEdit : public QWidget
        {
            Q_OBJECT
        public:
            explicit SearchLineEdit(QWidget* parent);

            ~SearchLineEdit() override;

            void setSearchHint(const std::string& searchHint);

            void setSearchIcon(const QIcon& icon);

        signals:
            void textChanged(const QString& text);

        private:
            std::unique_ptr<Ui::SearchLineEdit> m_ui;
        };
    } // namespace gui
} // namespace aide

#endif // AIDE_SEARCHLINEEDIT_HPP
