#ifndef AIDE_SETTINGS_DIALOG_INTERFACE_HPP
#define AIDE_SETTINGS_DIALOG_INTERFACE_HPP

#include <memory>
#include <string>

#include <QKeySequence>
#include <QList>
#include <QStringList>

#include <aide/helptopicid.hpp>

#include "settings/geometryandstatedata.hpp"
#include "userselection.hpp"

class QAbstractItemModel;
class QWidget;
class QByteArray;

namespace aide::core
{
    class SettingsDialogInterface
    {
    public:
        virtual ~SettingsDialogInterface() = default;

        virtual void setTreeModel(
            std::shared_ptr<QAbstractItemModel> model) = 0;

        virtual void restoreGeometryAndState(
            SettingsDialogGeometryAndStateData geometryAndStateData) = 0;

        virtual void setSelectedGroupIndex(const QModelIndex& index) = 0;

        [[nodiscard]] virtual SettingsDialogGeometryAndStateData
        currentGeometry() const = 0;

        virtual aide::core::UserSelection executeDialog() = 0;

        virtual void setSelectedPageDisplayName(
            const std::string& displayName) = 0;

        virtual void showSelectedPageWidget(QWidget* widget) = 0;

        virtual void showEmptyPageWidget() = 0;

        virtual void setSearchHistory(
            [[maybe_unused]] const QStringList& entries)
        {}

        virtual void showResetLabel(bool show) = 0;

        virtual void enableApplyButton(bool enable) = 0;

        // Hidden while help is inactive (Feature::Help off or no Help Base
        // URL).
        virtual void showHelpButton(bool show) = 0;

        // Enabled only while the selected page has an available Help Topic.
        virtual void enableHelpButton(bool enable) = 0;

        // Binds the selected page's Help Topic to the dialog itself, so
        // Context Help from anywhere in the dialog, the page tree included,
        // resolves to it. An empty ID clears it. A widget inside the page
        // with its own topic still wins.
        virtual void setHelpTopic(const HelpTopicId& topic) = 0;

        // The keys of the dialog's own Context Help shortcut, needed because
        // the main window's shortcut does not fire while the modal dialog is
        // open. An empty list means the dialog has no such shortcut.
        virtual void setContextHelpShortcuts(
            const QList<QKeySequence>& shortcuts) = 0;
    };

    using SettingsDialogWeakPtr = std::weak_ptr<SettingsDialogInterface>;

} // namespace aide::core
#endif // AIDE_SETTINGS_DIALOG_INTERFACE_HPP
