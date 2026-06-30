#include "aide/appearancemanager.hpp"

#include <algorithm>
#include <stdexcept>

#include <QApplication>
#include <QColor>
#include <QIcon>
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
#include <QGuiApplication>
#include <QStyleHints>
#endif

#include "aide/hierarchicalid.hpp"
#include "aide/settingsinterface.hpp"

namespace
{
    constexpr auto SYSTEM_THEME_NAME        = "System";
    constexpr auto LIGHT_THEME_NAME         = "Light";
    constexpr auto DARK_THEME_NAME          = "Dark";
    constexpr auto LIGHT_ICON_THEME         = "aide-light";
    constexpr auto DARK_ICON_THEME          = "aide-dark";
    constexpr auto BUILTIN_ICON_SEARCH_PATH = ":/aide/icons";

    constexpr double LUMINANCE_LIGHT_THRESHOLD = 0.5;

    // Named constants for light palette colors
    namespace lp
    {
        constexpr int WINDOW    = 240;
        constexpr int ALT_R     = 233;
        constexpr int ALT_G     = 231;
        constexpr int ALT_B     = 227;
        constexpr int TOOLTIP_R = 255;
        constexpr int TOOLTIP_G = 255;
        constexpr int TOOLTIP_B = 220;
        constexpr int LINK_B    = 255;
        constexpr int HILIT_R   = 48;
        constexpr int HILIT_G   = 140;
        constexpr int HILIT_B   = 198;
        constexpr int DISABLED  = 120;
        constexpr int DIS_HILIT = 145;
    } // namespace lp

    // Named constants for dark palette colors
    namespace dp
    {
        constexpr int WINDOW    = 53;
        constexpr int BASE      = 25;
        constexpr int HILIT_R   = 42;
        constexpr int HILIT_G   = 130;
        constexpr int HILIT_B   = 218;
        constexpr int DISABLED  = 127;
        constexpr int DIS_HILIT = 80;
    } // namespace dp

    struct SettingsKeys
    {
        aide::HierarchicalId theme =
            aide::HierarchicalId("Appearance")("Theme");
        aide::HierarchicalId fontFamily =
            aide::HierarchicalId("Appearance")("Font")("Family");
        aide::HierarchicalId fontSize =
            aide::HierarchicalId("Appearance")("Font")("Size");
    };

    const SettingsKeys& settingsKeys()
    {
        try {
            static const SettingsKeys keys;
            return keys;
        }
        catch (...) {
            std::terminate();
        }
    }
} // namespace

using aide::AppearanceManager;
using aide::ColorScheme;
using aide::Theme;

AppearanceManager::AppearanceManager(
    std::shared_ptr<SettingsInterface> settings, QObject* parent)
    : QObject(parent)
    , m_settings(std::move(settings))
{
    m_themes.push_back(
        {SYSTEM_THEME_NAME, QPalette{}, "", {BUILTIN_ICON_SEARCH_PATH}});
    m_themes.push_back({LIGHT_THEME_NAME,
                        lightPalette(),
                        DARK_ICON_THEME,
                        {BUILTIN_ICON_SEARCH_PATH}});
    m_themes.push_back({DARK_THEME_NAME,
                        darkPalette(),
                        LIGHT_ICON_THEME,
                        {BUILTIN_ICON_SEARCH_PATH}});

    restoreFromSettings();
}

void AppearanceManager::registerTheme(Theme theme)
{
    const auto isDuplicate = std::ranges::any_of(
        m_themes,
        [&theme](const auto& existing) { return existing.name == theme.name; });
    if (isDuplicate) {
        throw std::invalid_argument(
            "AppearanceManager: duplicate theme name '" +
            theme.name.toStdString() + "'");
    }
    m_themes.push_back(std::move(theme));
}

void AppearanceManager::addIconSearchPath(const QString& themeName,
                                          const QString& searchPath)
{
    findTheme(themeName).iconSearchPaths.append(searchPath);
}

QStringList AppearanceManager::themeNames() const
{
    QStringList names;
    names.reserve(static_cast<QStringList::size_type>(m_themes.size()));
    for (const auto& theme : m_themes) {
        names << theme.name;
    }
    return names;
}

QString AppearanceManager::activeThemeName() const
{
    return m_activeThemeName;
}

QFont AppearanceManager::activeFont() const
{
    return m_activeFont;
}

ColorScheme AppearanceManager::colorScheme() const
{
    if (m_activeThemeName == SYSTEM_THEME_NAME) {
        return schemeFromPalette(QApplication::palette());
    }
    return schemeFromPalette(findTheme(m_activeThemeName).palette);
}

void AppearanceManager::applyAppearance(const QString& themeName,
                                        const QString& fontFamily,
                                        int fontSizePoints)
{
    const auto& theme = findTheme(themeName);

    const auto oldScheme = colorScheme();

    m_activeThemeName = themeName;
    m_activeFont      = QFont{fontFamily, fontSizePoints};

    applyTheme(theme);
    applyIconSettings(theme);
    QApplication::setFont(m_activeFont);

    m_settings->setValue(settingsKeys().theme, themeName);
    m_settings->setValue(settingsKeys().fontFamily, fontFamily);
    m_settings->setValue(settingsKeys().fontSize, fontSizePoints);

    const auto newScheme = colorScheme();
    // cppcheck-suppress knownConditionTrueFalse
    if (newScheme != oldScheme) { emit colorSchemeChanged(newScheme); }
    emit appearanceChanged();

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    updateSystemThemeConnection();
#endif
}

// static
QPalette AppearanceManager::lightPalette()
{
    QPalette pal;
    pal.setColor(QPalette::Window, QColor(lp::WINDOW, lp::WINDOW, lp::WINDOW));
    pal.setColor(QPalette::WindowText, Qt::black);
    pal.setColor(QPalette::Base, Qt::white);
    pal.setColor(QPalette::AlternateBase,
                 QColor(lp::ALT_R, lp::ALT_G, lp::ALT_B));
    pal.setColor(QPalette::ToolTipBase,
                 QColor(lp::TOOLTIP_R, lp::TOOLTIP_G, lp::TOOLTIP_B));
    pal.setColor(QPalette::ToolTipText, Qt::black);
    pal.setColor(QPalette::Text, Qt::black);
    pal.setColor(QPalette::Button, QColor(lp::WINDOW, lp::WINDOW, lp::WINDOW));
    pal.setColor(QPalette::ButtonText, Qt::black);
    pal.setColor(QPalette::BrightText, Qt::red);
    pal.setColor(QPalette::Link, QColor(0, 0, lp::LINK_B));
    pal.setColor(QPalette::Highlight,
                 QColor(lp::HILIT_R, lp::HILIT_G, lp::HILIT_B));
    pal.setColor(QPalette::HighlightedText, Qt::white);
    pal.setColor(QPalette::Disabled, QPalette::WindowText,
                 QColor(lp::DISABLED, lp::DISABLED, lp::DISABLED));
    pal.setColor(QPalette::Disabled, QPalette::Text,
                 QColor(lp::DISABLED, lp::DISABLED, lp::DISABLED));
    pal.setColor(QPalette::Disabled, QPalette::ButtonText,
                 QColor(lp::DISABLED, lp::DISABLED, lp::DISABLED));
    pal.setColor(QPalette::Disabled, QPalette::Highlight,
                 QColor(lp::DIS_HILIT, lp::DIS_HILIT, lp::DIS_HILIT));
    return pal;
}

// static
QPalette AppearanceManager::darkPalette()
{
    QPalette pal;
    pal.setColor(QPalette::Window, QColor(dp::WINDOW, dp::WINDOW, dp::WINDOW));
    pal.setColor(QPalette::WindowText, Qt::white);
    pal.setColor(QPalette::Base, QColor(dp::BASE, dp::BASE, dp::BASE));
    pal.setColor(QPalette::AlternateBase,
                 QColor(dp::WINDOW, dp::WINDOW, dp::WINDOW));
    pal.setColor(QPalette::ToolTipBase,
                 QColor(dp::WINDOW, dp::WINDOW, dp::WINDOW));
    pal.setColor(QPalette::ToolTipText, Qt::white);
    pal.setColor(QPalette::Text, Qt::white);
    pal.setColor(QPalette::Button, QColor(dp::WINDOW, dp::WINDOW, dp::WINDOW));
    pal.setColor(QPalette::ButtonText, Qt::white);
    pal.setColor(QPalette::BrightText, Qt::red);
    pal.setColor(QPalette::Link, QColor(dp::HILIT_R, dp::HILIT_G, dp::HILIT_B));
    pal.setColor(QPalette::Highlight,
                 QColor(dp::HILIT_R, dp::HILIT_G, dp::HILIT_B));
    pal.setColor(QPalette::HighlightedText, Qt::black);
    pal.setColor(QPalette::Disabled, QPalette::WindowText,
                 QColor(dp::DISABLED, dp::DISABLED, dp::DISABLED));
    pal.setColor(QPalette::Disabled, QPalette::Text,
                 QColor(dp::DISABLED, dp::DISABLED, dp::DISABLED));
    pal.setColor(QPalette::Disabled, QPalette::ButtonText,
                 QColor(dp::DISABLED, dp::DISABLED, dp::DISABLED));
    pal.setColor(QPalette::Disabled, QPalette::Highlight,
                 QColor(dp::DIS_HILIT, dp::DIS_HILIT, dp::DIS_HILIT));
    return pal;
}

// static
ColorScheme AppearanceManager::schemeFromPalette(const QPalette& palette)
{
    const auto bg    = palette.color(QPalette::Window);
    const auto red   = static_cast<double>(bg.redF());
    const auto green = static_cast<double>(bg.greenF());
    const auto blue  = static_cast<double>(bg.blueF());
    const double lum = (0.2126 * red) + (0.7152 * green) + (0.0722 * blue);
    return lum > LUMINANCE_LIGHT_THRESHOLD ? ColorScheme::Light
                                           : ColorScheme::Dark;
}

// static
void AppearanceManager::applyTheme(const Theme& theme)
{
    if (theme.name == SYSTEM_THEME_NAME) {
        QApplication::setPalette(QPalette{});
    } else {
        QApplication::setPalette(theme.palette);
    }
}

// static
void AppearanceManager::applyIconSettings(const Theme& theme)
{
    QString iconThemeName = theme.iconThemeName;
    if (theme.name == SYSTEM_THEME_NAME) {
        const auto scheme = schemeFromPalette(QApplication::palette());
        iconThemeName =
            (scheme == ColorScheme::Light) ? DARK_ICON_THEME : LIGHT_ICON_THEME;
    }

    if (!iconThemeName.isEmpty()) {
        QIcon::setThemeName(iconThemeName);
        QIcon::setThemeSearchPaths(theme.iconSearchPaths);
    }
}

Theme& AppearanceManager::findTheme(const QString& name)
{
    const auto it = std::ranges::find_if(
        m_themes, [&name](const auto& thm) { return thm.name == name; });
    if (it == m_themes.end()) {
        throw std::invalid_argument("AppearanceManager: unknown theme '" +
                                    name.toStdString() + "'");
    }
    return *it;
}

const Theme& AppearanceManager::findTheme(const QString& name) const
{
    const auto it = std::ranges::find_if(
        m_themes, [&name](const auto& thm) { return thm.name == name; });
    if (it == m_themes.end()) {
        throw std::invalid_argument("AppearanceManager: unknown theme '" +
                                    name.toStdString() + "'");
    }
    return *it;
}

void AppearanceManager::restoreFromSettings()
{
    const auto appFont = QApplication::font();
    const QString savedTheme =
        m_settings->value(settingsKeys().theme, QString{SYSTEM_THEME_NAME})
            .toString();
    const QString family =
        m_settings->value(settingsKeys().fontFamily, appFont.family())
            .toString();
    const int size =
        m_settings->value(settingsKeys().fontSize, appFont.pointSize()).toInt();

    const auto it = std::ranges::find_if(
        m_themes,
        [&savedTheme](const auto& thm) { return thm.name == savedTheme; });
    const auto& theme = (it != m_themes.end()) ? *it : m_themes.front();

    m_activeThemeName = theme.name;
    m_activeFont      = QFont{family, size};

    applyTheme(theme);
    applyIconSettings(theme);
    QApplication::setFont(m_activeFont);

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    updateSystemThemeConnection();
#endif
}

void AppearanceManager::reapplySystemTheme()
{
    const auto oldScheme    = colorScheme();
    const auto& systemTheme = findTheme(SYSTEM_THEME_NAME);
    applyTheme(systemTheme);
    applyIconSettings(systemTheme);
    const auto newScheme = colorScheme();
    // cppcheck-suppress knownConditionTrueFalse
    if (newScheme != oldScheme) { emit colorSchemeChanged(newScheme); }
    emit appearanceChanged();
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
void AppearanceManager::updateSystemThemeConnection()
{
    auto* hints = QGuiApplication::styleHints();
    if (m_activeThemeName == SYSTEM_THEME_NAME) {
        connect(hints, &QStyleHints::colorSchemeChanged, this,
                &AppearanceManager::onOsColorSchemeChanged,
                Qt::UniqueConnection);
    } else {
        disconnect(hints, &QStyleHints::colorSchemeChanged, this,
                   &AppearanceManager::onOsColorSchemeChanged);
    }
}

void AppearanceManager::onOsColorSchemeChanged()
{
    reapplySystemTheme();
}
#endif
