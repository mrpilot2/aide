#ifndef AIDE_SETTINGS_DIALOG_INTERFACE_HPP
#define AIDE_SETTINGS_DIALOG_INTERFACE_HPP

#include <memory>

class QAbstractItemModel;

namespace aide::core
{
    class SettingsDialogInterface
    {
    public:
        virtual ~SettingsDialogInterface() = default;

        virtual void setTreeModel(
            std::shared_ptr<QAbstractItemModel> model) = 0;

        virtual void executeDialog() = 0;
    };

    using SettingsDialogWeakPtr = std::weak_ptr<SettingsDialogInterface>;

} // namespace aide::core
#endif // AIDE_SETTINGS_DIALOG_INTERFACE_HPP
