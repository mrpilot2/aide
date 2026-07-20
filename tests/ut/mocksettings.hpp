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
        // Keyed on the resolved name string, not HierarchicalId itself:
        // HierarchicalId stores raw const char* levels rather than owning
        // copies (see hierarchicalid.hpp), so a HierarchicalId built from a
        // temporary (e.g. a std::string::c_str()) dangles once that
        // temporary is gone. A long-lived map key must not be one.
        std::map<std::string, QVariant> inMemorySettings;
    };
} // namespace aide::test

#endif // AIDE_MOCK_SETTINGS_HPP
