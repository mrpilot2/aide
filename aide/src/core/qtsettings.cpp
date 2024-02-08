
#include "qtsettings.hpp"

#include <QApplication>
#include <QSettings>
#include <QString>

#include "aide/hierarchicalid.hpp"

using aide::QtSettings;

QtSettings::QtSettings(const bool versionable)
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

void QtSettings::setValue(const HierarchicalId& groupAndKey,
                          const QVariant& value)
{
    const auto* key = *(groupAndKey.end() - 1);
    const auto group =
        HierarchicalId(groupAndKey.begin(), groupAndKey.end() - 1);

    for (const auto* g : group) {
        m_settings->beginGroup(QString::fromLatin1(g));
    }
    m_settings->setValue(QString::fromStdString(key), value);
    for ([[maybe_unused]] const auto* g : group) {
        m_settings->endGroup();
    }
}

QVariant QtSettings::value(const HierarchicalId& groupAndKey)
{
    return value(groupAndKey, QVariant());
}

QVariant QtSettings::value(const HierarchicalId& groupAndKey,
                           const QVariant& defaultValue)
{
    const auto* key = *(groupAndKey.end() - 1);
    const auto group =
        HierarchicalId(groupAndKey.begin(), groupAndKey.end() - 1);

    for (const auto* g : group) {
        m_settings->beginGroup(QString::fromLatin1(g));
    }

    auto settingsValue =
        m_settings->value(QString::fromStdString(std::string(key)));

    for ([[maybe_unused]] const auto* g : group) {
        m_settings->endGroup();
    }

    return settingsValue != QVariant() ? settingsValue : defaultValue;
}

void QtSettings::removeKey(const HierarchicalId& groupAndKey)
{
    const auto* key = *(groupAndKey.end() - 1);
    const auto group =
        HierarchicalId(groupAndKey.begin(), groupAndKey.end() - 1);

    for (const auto* g : group) {
        m_settings->beginGroup(QString::fromLatin1(g));
    }

    m_settings->remove(key);

    for ([[maybe_unused]] const auto* g : group) {
        m_settings->endGroup();
    }
}

void QtSettings::save()
{
    m_settings->sync();
}
void QtSettings::load()
{
    m_settings->sync();
}
