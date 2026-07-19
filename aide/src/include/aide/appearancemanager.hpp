#ifndef AIDE_APPEARANCE_MANAGER_HPP
#define AIDE_APPEARANCE_MANAGER_HPP

#include <memory>
#include <vector>

#include <QColor>
#include <QFont>
#include <QObject>
#include <QPalette>
#include <QStringList>

#include <aide/colorscheme.hpp>
#include <aide/notificationtype.hpp>
#include <aide/theme.hpp>

namespace aide
{
    class SettingsInterface;

    class AppearanceManager : public QObject
    {
        Q_OBJECT

    public:
        explicit AppearanceManager(std::shared_ptr<SettingsInterface> settings,
                                   QObject* parent = nullptr);

        void registerTheme(Theme theme);
        void addIconSearchPath(const QString& themeName,
                               const QString& searchPath);

        [[nodiscard]] QStringList themeNames() const;
        [[nodiscard]] QString activeThemeName() const;
        [[nodiscard]] QFont activeFont() const;
        [[nodiscard]] ColorScheme colorScheme() const;
        [[nodiscard]] QColor severityColor(NotificationType type) const;

        void applyAppearance(const QString& themeName,
                             const QString& fontFamily, int fontSizePoints);

    signals:
        void colorSchemeChanged(aide::ColorScheme scheme);
        void appearanceChanged();

    private:
        static QPalette lightPalette();
        static QPalette darkPalette();
        static ColorScheme schemeFromPalette(const QPalette& palette);
        static void applyTheme(const Theme& theme);

        static void applyIconSettings(const Theme& theme);

        Theme& findTheme(const QString& name);
        const Theme& findTheme(const QString& name) const;
        void restoreFromSettings();
        void honorPersistedThemeIfPending(const QString& themeName);
        void reapplySystemTheme();

        std::shared_ptr<SettingsInterface> m_settings;
        std::vector<Theme> m_themes;
        QString m_activeThemeName;
        QFont m_activeFont;

        void updateSystemThemeConnection();

    private slots:
        void onOsColorSchemeChanged();
    };
} // namespace aide

#endif // AIDE_APPEARANCE_MANAGER_HPP
