
#include "keymappagewidget.hpp"

#include <utility>

#include <QMenu>

#include "settings/keymap/keymapcontextmenuprovider.hpp"
#include "ui_keymappagewidget.h"

using aide::gui::KeymapPageWidget;

KeymapPageWidget::KeymapPageWidget(QWidget* parent)
    : QWidget(parent)
    , ui(std::make_unique<Ui::KeymapPageWidget>())
{
    ui->setupUi(this);

    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
}

KeymapPageWidget::~KeymapPageWidget() = default;

void KeymapPageWidget::setController(KeyMapPageControllerPtr controller)
{
    keymapPageController = std::move(controller);
    connectSignals();
}

void aide::gui::KeymapPageWidget::connectSignals() const
{
    connect(
        ui->treeView, &QTreeView::doubleClicked, keymapPageController.get(),
        &KeyMapPageWidgetController::onUserRequestedContextMenuViaDoubleClick);
    connect(ui->treeView, &QTreeView::customContextMenuRequested, this,
            &KeymapPageWidget::onUserRequestedContextMenuViaRightClick);
}

void KeymapPageWidget::onUserRequestedContextMenuViaRightClick(
    const QPoint& point)
{
    QModelIndex index = ui->treeView->indexAt(point);
    keymapPageController->requestContextMenuForIndex(index);
}

void aide::gui::KeymapPageWidget::setTreeModel(
    std::shared_ptr<QAbstractItemModel> model)
{
    ui->treeView->setModel(model.get());

    ui->treeView->expandAll();
    ui->treeView->resizeColumnToContents(0);
    ui->treeView->collapseAll();
}
void aide::gui::KeymapPageWidget::showContextMenu(
    const aide::core::ContextMenuEntries& entries)
{
    QMenu menu(ui->treeView);

    for (const auto& entry : entries) {
        switch (entry.type) {
        case aide::core::ContextMenuItemType::SEPARATOR:
            menu.addSeparator();
            break;
        default:
            menu.addAction(QString::fromStdString(entry.displayText));
            break;
        }
    }

    menu.exec(QCursor::pos());
}
