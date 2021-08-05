
#include "keymappagewidget.hpp"

#include "ui_keymappagewidget.h"

using aide::gui::KeymapPageWidget;

KeymapPageWidget::KeymapPageWidget(QWidget* parent)
    : QWidget(parent)
    , ui(std::make_unique<Ui::KeymapPageWidget>())
{
    ui->setupUi(this);
}

KeymapPageWidget::~KeymapPageWidget() = default;

void aide::gui::KeymapPageWidget::setTreeModel(
    std::shared_ptr<QAbstractItemModel> model)
{
    ui->treeView->setModel(model.get());

    ui->treeView->expandAll();
    ui->treeView->resizeColumnToContents(0);
    ui->treeView->collapseAll();
}

void aide::gui::KeymapPageWidget::showContextMenu([
    [maybe_unused]] const aide::core::ContextMenuEntries& entries)
{}
