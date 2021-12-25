
#ifndef AIDE_AIDE_TABLE_VIEW_HPP
#define AIDE_AIDE_TABLE_VIEW_HPP

#include <QTableView>

namespace aide::widgets
{
    class AideTableView : public QTableView
    {
    public:
        explicit AideTableView(QWidget* parent);

        void setPlaceHolderText(QString text);

    protected:
        void paintEvent(QPaintEvent* event) override;

    private:
        QString placeHolderText;
    };
} // namespace aide::widgets

#endif // AIDE_AIDE_TABLE_VIEW_HPP
