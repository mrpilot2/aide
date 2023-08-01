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
        explicit QtSettings(bool versionable);
        ~QtSettings() override;

        void setValue(const HierarchicalId& groupAndKey,
                      const QVariant& value) override;

        QVariant value(const HierarchicalId& groupAndKey) override;

        QVariant value(const HierarchicalId& groupAndKey,
                       const QVariant& defaultValue) override;

        void removeKey(const HierarchicalId& groupAndKey) override;

        void save() override;

        void load() override;

    private:
        std::unique_ptr<QSettings> m_settings;
    };
} // namespace aide

#endif // AIDE_QT_SETTINGS_HPP
