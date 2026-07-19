#include "notificationpopuptypedelegate.hpp"

#include <array>

#include <QComboBox>

#include <aide/notificationdisplaytype.hpp>

#include "settings/notifications/notificationgrouptablemodel.hpp"

using aide::NotificationDisplayType;
using aide::core::NotificationGroupTableModel;
using aide::gui::NotificationPopupTypeDelegate;

namespace
{
    constexpr std::array<NotificationDisplayType, 3> DISPLAY_TYPES{
        NotificationDisplayType::None, NotificationDisplayType::Balloon,
        NotificationDisplayType::StickyBalloon};

    constexpr int NO_MATCH_INDEX = -1;
    constexpr int FALLBACK_INDEX = 0;
} // namespace

NotificationPopupTypeDelegate::NotificationPopupTypeDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{}

QWidget* NotificationPopupTypeDelegate::createEditor(
    QWidget* parent, const QStyleOptionViewItem& /*option*/,
    const QModelIndex& /*index*/) const
{
    auto* combo = new QComboBox(parent);
    for (const auto type : DISPLAY_TYPES) {
        combo->addItem(NotificationGroupTableModel::displayTypeLabel(type),
                       static_cast<int>(type));
    }
    return combo;
}

void NotificationPopupTypeDelegate::setEditorData(
    QWidget* editor, const QModelIndex& index) const
{
    auto* combo = qobject_cast<QComboBox*>(editor);
    if (combo == nullptr) { return; }

    const auto value      = index.data(Qt::EditRole).toInt();
    const auto comboIndex = combo->findData(value);
    combo->setCurrentIndex(comboIndex != NO_MATCH_INDEX ? comboIndex
                                                        : FALLBACK_INDEX);
}

void NotificationPopupTypeDelegate::setModelData(QWidget* editor,
                                                 QAbstractItemModel* model,
                                                 const QModelIndex& index) const
{
    auto* combo = qobject_cast<QComboBox*>(editor);
    if (combo == nullptr) { return; }

    model->setData(index, combo->currentData(), Qt::EditRole);
}

void NotificationPopupTypeDelegate::updateEditorGeometry(
    QWidget* editor, const QStyleOptionViewItem& option,
    const QModelIndex& /*index*/) const
{
    editor->setGeometry(option.rect);
}
