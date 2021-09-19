
#ifndef AIDE_AIDE_TREE_VIEW_HPP
#define AIDE_AIDE_TREE_VIEW_HPP

#include <QTreeView>

namespace aide::widgets
{
    class AideTreeView : public QTreeView
    {
    public:
        explicit AideTreeView(QWidget* parent);

        void setPlaceHolderText(QString text);

    protected:
        void paintEvent(QPaintEvent* event) override;

    private:
        QString placeHolderText;
    };
} // namespace aide::widgets

#endif // AIDE_AIDE_TREE_VIEW_HPP
