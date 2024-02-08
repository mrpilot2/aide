
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

void KeymapPageWidget::connectSignals() const
{
    connect(
        ui->treeView, &QTreeView::doubleClicked, keymapPageController.get(),
        &KeyMapPageWidgetController::onUserRequestedContextMenuViaDoubleClick);
    connect(ui->treeView, &QTreeView::customContextMenuRequested, this,
            &KeymapPageWidget::onUserRequestedContextMenuViaRightClick);
}

void KeymapPageWidget::onUserRequestedContextMenuViaRightClick(
    const QPoint& point) const
{
    const QModelIndex index = ui->treeView->indexAt(point);
    keymapPageController->requestContextMenuForIndex(index);
}

void KeymapPageWidget::setTreeModel(
    const std::shared_ptr<QAbstractItemModel> model)
{
    const bool isFirstModelSet(ui->treeView->model() == nullptr);

    ui->treeView->setModel(model.get());

    if (isFirstModelSet) {
        ui->treeView->expandAll();
        ui->treeView->resizeColumnToContents(0);
        ui->treeView->collapseAll();
    }
}

void KeymapPageWidget::showContextMenu(const core::ContextMenuEntries& entries)
{
    QMenu menu(ui->treeView);

    for (const auto& entry : entries) {
        switch (entry.type) {
        case core::ContextMenuItemType::SEPARATOR:
            menu.addSeparator();
            break;
        case core::ContextMenuItemType::REMOVE_SHORTCUT:
        {
            auto* action =
                new QAction(QString::fromStdString(entry.displayText), &menu);
            action->setData(entry.sequence);
            QObject::connect(
                action, &QAction::triggered, keymapPageController.get(),
                &KeyMapPageWidgetController::onUserRequestedToRemoveAShortcut);
            menu.addAction(action);
            break;
        }
        case core::ContextMenuItemType::RESET_TO_DEFAULTS:
            menu.addAction(QString::fromStdString(entry.displayText),
                           keymapPageController.get(),
                           &KeyMapPageWidgetController::
                               onUserRequestedToResetCurrentShortcutsToDefault);
            break;
        case core::ContextMenuItemType::ADD_KEYBOARD_SHORTCUT:
            menu.addAction(QString::fromStdString(entry.displayText),
                           keymapPageController.get(),
                           &KeyMapPageWidgetController::
                               onUserRequestedToAddAKeyboardShortcut);
            break;
        default:
            menu.addAction(QString::fromStdString(entry.displayText));
            break;
        }
    }

    menu.exec(QCursor::pos());
}
