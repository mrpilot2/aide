
#include "qtsettings.hpp"

#include <QApplication>
#include <QSettings>
#include <QString>

#include "aide/hierarchicalid.hpp"

using aide::QtSettings;

QtSettings::QtSettings(bool versionable)
{
    if (versionable) {
        m_settings = std::make_unique<QSettings>();
    } else {
        m_settings = std::make_unique<QSettings>(
            QApplication::organizationName(),
            QApplication::applicationName() + "_unversionable");
    }
}

QtSettings::~QtSettings() = default;

void QtSettings::setValue(const aide::HierarchicalId& group,
                          const std::string& key, const QVariant& value)
{
    for (const auto* g : group) {
        m_settings->beginGroup(QString::fromLatin1(g));
    }
    m_settings->setValue(QString::fromStdString(key), value);
    for ([[maybe_unused]] const auto* g : group) {
        m_settings->endGroup();
    }
}

QVariant QtSettings::value(const aide::HierarchicalId& group,
                           const std::string& key)
{
    for (const auto* g : group) {
        m_settings->beginGroup(QString::fromLatin1(g));
    }

    auto settingsValue = m_settings->value(QString::fromStdString(key));

    for ([[maybe_unused]] const auto* g : group) {
        m_settings->endGroup();
    }

    return settingsValue;
}

QVariant QtSettings::value(const aide::HierarchicalId& group, std::string key,
                           const QVariant& defaultValue)
{
    if (auto val = this->value(group, key) != QVariant()) { return val; }
    return defaultValue;
}

void aide::QtSettings::save()
{
    m_settings->sync();
}

void aide::QtSettings::load()
{
    m_settings->sync();
}
