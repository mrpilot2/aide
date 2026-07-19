#ifndef AIDE_NOTIFICATIONS_SETTINGS_PAGE_HPP
#define AIDE_NOTIFICATIONS_SETTINGS_PAGE_HPP

#include <vector>

#include <QWidget>

#include <aide/notificationdisplaytype.hpp>
#include <aide/settings/settingspage.hpp>

class QCheckBox;
class QComboBox;

namespace aide
{
    class NotificationManagerInterface;
    class SettingsInterface;

    namespace widgets
    {
        class AideTableView;
    } // namespace widgets

    namespace core
    {
        class NotificationGroupTableModel;
    } // namespace core
} // namespace aide

namespace aide::gui
{
    // The notification settings page (#154): Do Not Disturb + balloon
    // placement above a flat table with one row per registered
    // NotificationGroup. Merges widget and SettingsPage into a single
    // object, mirroring AppearancePage - the page's logic is thin enough
    // that a separate core/gui split (as with KeymapPage) isn't warranted.
    class NotificationsSettingsPage
        : public QWidget
        , public core::SettingsPage
    {
        Q_OBJECT

    public:
        NotificationsSettingsPage(NotificationManagerInterface& manager,
                                  SettingsInterface& settings,
                                  QWidget* parent = nullptr);

        QWidget* widget() override;
        [[nodiscard]] bool isModified() const override;
        [[nodiscard]] QStringList groupTitles() const override;
        void reset() override;
        void apply() override;

        [[nodiscard]] bool matches(const QString& pattern) override;
        [[nodiscard]] double score(const QStringList& words) override;
        void highlight(const QString& pattern) override;

    private:
        void syncControlsFromSettings();
        [[nodiscard]] std::vector<NotificationDisplayType> currentDisplayTypes()
            const;

        NotificationManagerInterface& m_manager;
        SettingsInterface& m_settings;

        QCheckBox* m_doNotDisturbCheckBox{nullptr};
        QComboBox* m_placementCombo{nullptr};
        widgets::AideTableView* m_tableView{nullptr};
        core::NotificationGroupTableModel* m_model{nullptr};

        bool m_appliedDoNotDisturb{false};
        int m_appliedPlacementIndex{0};
        std::vector<NotificationDisplayType> m_appliedDisplayTypes;
    };
} // namespace aide::gui

#endif // AIDE_NOTIFICATIONS_SETTINGS_PAGE_HPP
