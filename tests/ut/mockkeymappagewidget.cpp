

#include "mockkeymappagewidget.hpp"

#include <QAbstractItemModel>

aide::test::MockKeyMapPageWidget::MockKeyMapPageWidget(QWidget* parent)
    : QWidget(parent)
{}

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

void aide::test::MockKeyMapPageWidget::showContextMenu(
    const aide::core::ContextMenuEntries& entries)
{
    contextMenuWasSet             = true;
    numberOfSetContextMenuEntries = entries.size();
}

bool aide::test::MockKeyMapPageWidget::wasContextMeuSet() const
{
    return contextMenuWasSet;
}

size_t aide::test::MockKeyMapPageWidget::numberOfContextMenuEntries() const
{
    return numberOfSetContextMenuEntries;
}
