
#include "keymappagewidgetcontroller.hpp"

#include <iostream>

#include <QAction>
#include <QDialog>

#include "addshortcutdialog.hpp"
#include "settings/keymap/keymapcontextmenuprovider.hpp"
#include "settings/keymap/keymaptreemodel.hpp"

using aide::core::KeymapContextMenuProvider;
using aide::core::KeyMapTreeModel;
using aide::gui::KeyMapPageWidgetController;

KeyMapPageWidgetController::KeyMapPageWidgetController(
    const std::shared_ptr<KeyMapTreeModel>& treeModel,
    core::KeyMapPageWidgetInterface* view)
    : keyMapTreeModel(treeModel)
    , keyMapView(view)
{}

void KeyMapPageWidgetController::requestContextMenuForIndex(
    const QModelIndex& index)
{
    currentIndex = keyMapTreeModel->index(index.row(), 1, index.parent());

    KeymapContextMenuProvider creator{keyMapTreeModel.get(), keyMapView};
    creator.createAndShowContextMenu(index);
}

void KeyMapPageWidgetController::onUserRequestedContextMenuViaDoubleClick(
    const QModelIndex& index)
{
    requestContextMenuForIndex(index);
}

void KeyMapPageWidgetController::
    onUserRequestedToResetCurrentShortcutsToDefault()
{
    if (auto action = keyMapTreeModel->findCorrespondingAction(currentIndex)) {
        keyMapTreeModel->setData(
            currentIndex,
            QKeySequence::listToString(action->defaultKeySequences),
            Qt::DisplayRole);
    }
}

void KeyMapPageWidgetController::onUserRequestedToRemoveAShortcut()
{
    if (auto* action = qobject_cast<QAction*>(sender()); action != nullptr) {
        auto treeKeySequences = QKeySequence::listFromString(
            keyMapTreeModel->data(currentIndex, Qt::DisplayRole).toString());

        treeKeySequences.removeOne(action->data().value<QKeySequence>());

        keyMapTreeModel->setData(currentIndex,
                                 QKeySequence::listToString(treeKeySequences),
                                 Qt::DisplayRole);
    }
}

void aide::gui::KeyMapPageWidgetController::
    onUserRequestedToAddAKeyboardShortcut()
{
    if (auto action = keyMapTreeModel->findCorrespondingAction(currentIndex)) {
        AddShortcutDialog dialog;

        dialog.setAssignedActionDescription(
            QString::fromStdString(action->description));

        if (auto res = dialog.exec(); res == QDialog::Accepted) {
            const auto treeModelData =
                keyMapTreeModel->data(currentIndex, Qt::DisplayRole).toString();

            auto treeKeySequences =
                !treeModelData.isEmpty()
                    ? QKeySequence::listFromString(treeModelData)
                    : QList<QKeySequence>();

            treeKeySequences.append(dialog.userSelectedKeySequence());

            keyMapTreeModel->setData(
                currentIndex, QKeySequence::listToString(treeKeySequences),
                Qt::DisplayRole);
        }
    }
}
