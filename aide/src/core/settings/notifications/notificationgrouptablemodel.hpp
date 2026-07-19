#ifndef AIDE_NOTIFICATION_GROUP_TABLE_MODEL_HPP
#define AIDE_NOTIFICATION_GROUP_TABLE_MODEL_HPP

#include <vector>

#include <QAbstractTableModel>
#include <QString>
#include <QStringList>

#include <aide/notificationdisplaytype.hpp>
#include <aide/notificationgroup.hpp>

namespace aide::core
{
    // One row per registered NotificationGroup, auto-generated so the
    // notification settings page (#154) scales to an arbitrary group count.
    // The model is dumb about persistence: NotificationsSettingsPage loads
    // and saves the per-row popup type via reset()/apply(), mirroring every
    // other SettingsPage.
    class NotificationGroupTableModel : public QAbstractTableModel
    {
        Q_OBJECT

    public:
        enum Column : int
        {
            GroupColumn = 0,
            PopupTypeColumn,
            LogColumn,
            ColumnCount
        };

        explicit NotificationGroupTableModel(QObject* parent = nullptr);

        // Replaces the rows, seeding each row's popup type from the group's
        // default. Callers that need to reflect a per-group settings
        // override call setDisplayTypeAt() afterwards.
        void setGroups(std::vector<NotificationGroup> groups);

        [[nodiscard]] int rowCount(
            const QModelIndex& parent = QModelIndex()) const override;

        [[nodiscard]] int columnCount(
            const QModelIndex& parent = QModelIndex()) const override;

        [[nodiscard]] QVariant data(const QModelIndex& index,
                                    int role) const override;

        bool setData(const QModelIndex& index, const QVariant& value,
                     int role) override;

        [[nodiscard]] Qt::ItemFlags flags(
            const QModelIndex& index) const override;

        [[nodiscard]] QVariant headerData(int section,
                                          Qt::Orientation orientation,
                                          int role) const override;

        [[nodiscard]] const NotificationGroup& groupAt(int row) const;

        [[nodiscard]] NotificationDisplayType displayTypeAt(int row) const;

        void setDisplayTypeAt(int row, NotificationDisplayType type);

        // Search integration (#154): rows are model items, not child
        // widgets, so NotificationsSettingsPage delegates its matches()/
        // score()/highlight() overrides here instead of the default
        // child-widget scan.
        [[nodiscard]] bool matchesAnyRow(const QStringList& words) const;

        [[nodiscard]] double rowsScore(const QStringList& words) const;

        // Stores the active search pattern so data() can report a
        // Qt::BackgroundRole highlight for matching rows; used by
        // NotificationsSettingsPage::highlight().
        void setSearchPattern(const QString& pattern);

        [[nodiscard]] static QString displayTypeLabel(
            NotificationDisplayType type);

    private:
        struct Row
        {
            NotificationGroup group;
            NotificationDisplayType displayType{NotificationDisplayType::None};
        };

        [[nodiscard]] static bool rowMatches(const Row& row,
                                             const QStringList& words);

        std::vector<Row> m_rows;
        QString m_searchPattern;
    };
} // namespace aide::core

#endif // AIDE_NOTIFICATION_GROUP_TABLE_MODEL_HPP
