#ifndef AIDE_MOCK_SETTINGS_HPP
#define AIDE_MOCK_SETTINGS_HPP

#include <aide/hierarchicalid.hpp>
#include <aide/settingsinterface.hpp>

namespace aide::test
{
    class MockSettings : public aide::SettingsInterface
    {
    public:
        void setValue(const HierarchicalId& groupAndKey,
                      const QVariant& value) override;

        QVariant value(const HierarchicalId& groupAndKey) override;

        QVariant value(const HierarchicalId& groupAndKey,
                       const QVariant& defaultValue) override;

        void removeKey(const HierarchicalId& key) override;

        void save() override;
        void load() override;

    private:
        std::map<HierarchicalId, QVariant> inMemorySettings;
    };
} // namespace aide::test

#endif // AIDE_MOCK_SETTINGS_HPP
