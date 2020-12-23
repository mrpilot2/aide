#ifndef AIDE_MOCK_SETTINGS_HPP
#define AIDE_MOCK_SETTINGS_HPP

#include "settingsinterface.hpp"

namespace aide::test
{
    class MockSettings : public aide::SettingsInterface
    {
    public:
        void setValue(const HierarchicalId& group, const std::string& key,
                      const QVariant& value) override;
        QVariant value(const HierarchicalId& group,
                       const std::string& key) override;
        QVariant value(const HierarchicalId& group, const std::string& key,
                       const QVariant& defaultValue) override;
        void save() override;
        void load() override;

    private:
        std::map<std::pair<HierarchicalId, std::string>, QVariant>
            inMemorySettings;
    };
} // namespace aide::test

#endif // AIDE_MOCK_SETTINGS_HPP
