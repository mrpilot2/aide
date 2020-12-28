#ifndef AIDE_COMMON_SETTINGS_KEYS_HPP
#define AIDE_COMMON_SETTINGS_KEYS_HPP

#include <aide/hierarchicalid.hpp>
#include <aide/logger.hpp>

#if defined(_MSC_VER)
#define ALIGNMENT(size) __declspec(align(size))
#elif defined(__GNUC__)
#define ALIGNMENT(size) __attribute__((aligned(size)))
#endif

namespace aide::core::settings
{
    struct Keys
    {
        struct System
        {
            const HierarchicalId ASK_EXIT_CONFIRMATION =
                HierarchicalId("System")("Behavior")("AskExitConfirmation");
        } ALIGNMENT(32);

        struct Ui
        {
            const HierarchicalId MAIN_WINDOW_GEOMETRY_KEY{
                HierarchicalId("Geometry")("MainWindow")};
            const HierarchicalId MAIN_WINDOW_STATE_KEY{
                HierarchicalId("State")("MainWindow")};
        } ALIGNMENT(64);

        const Ui UI{};
        const System SYSTEM{};
    } ALIGNMENT(128);

    const static Keys& KEYS()
    {
        try {
            static Keys keys;
            return keys;
        }
        catch (...) {
            AIDE_LOG_CRITICAL(
                "CommonSettingsKeys: could not create settings keys with "
                "static storage duration. This should only happen if the "
                "application is already out of memory at startup")
            std::terminate();
        }
    }
} // namespace aide::core::settings

#endif // AIDE_COMMON_SETTINGS_KEYS_HPP
