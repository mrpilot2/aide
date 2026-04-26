#ifndef AIDE_APPEARANCE_MANAGER_HPP
#define AIDE_APPEARANCE_MANAGER_HPP

#include <memory>
#include <vector>

#include <QFont>
#include <QObject>
#include <QPalette>
#include <QStringList>

#include <aide/colorscheme.hpp>
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

        [[nodiscard]] QStringList themeNames() const;
        [[nodiscard]] QString activeThemeName() const;
        [[nodiscard]] QFont activeFont() const;
        [[nodiscard]] ColorScheme colorScheme() const;

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

        const Theme& findTheme(const QString& name) const;
        void restoreFromSettings();
        void reapplySystemTheme();

        std::shared_ptr<SettingsInterface> m_settings;
        std::vector<Theme> m_themes;
        QString m_activeThemeName;
        QFont m_activeFont;

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        void updateSystemThemeConnection();

    private slots:
        void onOsColorSchemeChanged();
#endif
    };
} // namespace aide

#endif // AIDE_APPEARANCE_MANAGER_HPP
