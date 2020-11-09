#ifndef AIDE_QT_SETTINGS_HPP
#define AIDE_QT_SETTINGS_HPP

#include <memory>

#include <aide/settingsinterface.hpp>

class QSettings;

namespace aide
{
    class QtSettings : public SettingsInterface
    {
    public:
        QtSettings(bool versionable);
        ~QtSettings() override;

        void setValue(const HierarchicalId& group, const std::string& key,
                      const QVariant& value) override;

        QVariant value(const HierarchicalId& group,
                       const std::string& key) override;

        QVariant value(const HierarchicalId& group, std::string key,
                       const QVariant& defaultValue) override;

        void save() override;

        void load() override;

    private:
        std::unique_ptr<QSettings> m_settings;
    };
} // namespace aide

#endif // AIDE_QT_SETTINGS_HPP
