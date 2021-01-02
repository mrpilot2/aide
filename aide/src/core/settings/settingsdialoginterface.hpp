#ifndef AIDE_SETTINGS_DIALOG_INTERFACE_HPP
#define AIDE_SETTINGS_DIALOG_INTERFACE_HPP

#include <memory>
#include <string>

#include "userselection.hpp"

class QAbstractItemModel;
class QWidget;

namespace aide::core
{
    class SettingsDialogInterface
    {
    public:
        virtual ~SettingsDialogInterface() = default;

        virtual void setTreeModel(
            std::shared_ptr<QAbstractItemModel> model) = 0;

        virtual aide::core::UserSelection executeDialog() = 0;

        virtual void setSelectedPageDisplayName(
            const std::string& displayName) = 0;

        virtual void showSelectedPageWidget(QWidget* widget) = 0;

        virtual void showEmptyPageWidget() = 0;

        virtual void showResetLabel(bool show) = 0;

        virtual void enableApplyButton(bool enable) = 0;
    };

    using SettingsDialogWeakPtr = std::weak_ptr<SettingsDialogInterface>;

} // namespace aide::core
#endif // AIDE_SETTINGS_DIALOG_INTERFACE_HPP
