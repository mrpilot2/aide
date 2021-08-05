
#include "keymapcontextmenuprovider.hpp"

#include <QModelIndex>

#include "keymaptreemodel.hpp"

using aide::core::KeymapContextMenuProvider;
using aide::core::KeyMapPageWidgetRawPtr;

KeymapContextMenuProvider::KeymapContextMenuProvider(
    KeyMapTreeModel* model, KeyMapPageWidgetRawPtr view)
    : treeModel{model}
    , keymapView{view}
{}

void KeymapContextMenuProvider::createAndShowContextMenu(
    const QModelIndex& index)
{
    if (!index.isValid() || !treeModel->findCorrespondingAction(index)) {
        return;
    }

    ContextMenuEntries entries;
    entries.push_back(
        {ContextMenuItemType::ADD_KEYBOARD_SHORTCUT,
         KeymapContextMenuProvider::tr("Add keyboard shortcut").toStdString(),
         QKeySequence()});

    auto action = treeModel->findCorrespondingAction(index);

    auto* item = static_cast<TreeItem*>(index.internalPointer());
    auto keySequenceInModel{
        QKeySequence::listFromString(item->data(1).toString())};

    bool keySequencesAreAllEmpty{
        std::all_of(keySequenceInModel.begin(), keySequenceInModel.end(),
                    [](auto elem) { return elem.isEmpty(); })};
    if (!keySequencesAreAllEmpty) {
        entries.push_back({ContextMenuItemType::SEPARATOR, "", QKeySequence()});

        for (const auto& seq : keySequenceInModel) {
            if (seq.isEmpty()) { continue; }
            entries.push_back({ContextMenuItemType::REMOVE_SHORTCUT,
                               KeymapContextMenuProvider::tr("Remove %1")
                                   .arg(seq.toString())
                                   .toStdString(),
                               seq});
        }
    }

    if (!action->areKeySequencesTheSame(keySequenceInModel,
                                        action->defaultKeySequences)) {
        entries.push_back({ContextMenuItemType::SEPARATOR, "", QKeySequence()});

        entries.push_back(
            {ContextMenuItemType::RESET_TO_DEFAULTS,
             KeymapContextMenuProvider::tr("Reset to default").toStdString(),
             QKeySequence()});
    }
    keymapView->showContextMenu(entries);
}
