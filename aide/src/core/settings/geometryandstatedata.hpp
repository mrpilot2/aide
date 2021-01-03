#ifndef AIDE_GEOMETRY_AND_STATE_DATA_HPP
#define AIDE_GEOMETRY_AND_STATE_DATA_HPP

#include <QPersistentModelIndex>

namespace aide::core
{
    struct SettingsDialogGeometryAndStateData
    {
        QByteArray dialogGeometry{};
        QByteArray splitterGeometry{};
        QByteArray splitterState{};
        QByteArray treeViewGeometry{};
        QString selectedTreeViewItem{};

        bool operator==(const SettingsDialogGeometryAndStateData& rhs) const
        {
            return std::tie(dialogGeometry, splitterGeometry, splitterState,
                            treeViewGeometry, selectedTreeViewItem) ==
                   std::tie(rhs.dialogGeometry, rhs.splitterGeometry,
                            rhs.splitterState, rhs.treeViewGeometry,
                            rhs.selectedTreeViewItem);
        }
    };
} // namespace aide::core

#endif // AIDE_GEOMETRY_AND_STATE_DATA_HPP
