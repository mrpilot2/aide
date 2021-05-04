

#include "mockkeymappagewidget.hpp"

#include <QAbstractItemModel>

void aide::test::MockKeyMapPageWidget::setTreeModel(
    std::shared_ptr<QAbstractItemModel> model)
{
    treeModelWasSet = true;
    treeModel       = model;
}

bool aide::test::MockKeyMapPageWidget::wasTreeModelSet() const
{
    return treeModelWasSet;
}
int aide::test::MockKeyMapPageWidget::numberOfRowsInTree() const
{
    return treeModel->rowCount();
}
