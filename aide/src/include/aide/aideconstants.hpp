
#ifndef AIDE_AIDE_CONSTANTS_HPP
#define AIDE_AIDE_CONSTANTS_HPP

#include <aide/hierarchicalid.hpp>
#include <aide/log_helper_macros.hpp>

namespace aide::constants
{
    struct Constants
    {
        // default menus
        const HierarchicalId MAIN_MENU{HierarchicalId("Main Menu")};
        const HierarchicalId MENU_FILE{MAIN_MENU.addLevel("File")};
        const HierarchicalId MENU_HELP{MAIN_MENU.addLevel("Help")};

        // actions
        const HierarchicalId FILE_SETTINGS{MENU_FILE.addLevel("Settings")};
        const HierarchicalId FILE_QUIT{MENU_FILE.addLevel("Quit")};

        const HierarchicalId HELP_ABOUT_AIDE{MENU_HELP.addLevel("About Aide")};
        const HierarchicalId HELP_ABOUT_QT{MENU_HELP.addLevel("About Qt")};
    };

    const static Constants& CONSTANTS()
    {
        try {
            static const Constants ids;
            return ids;
        }
        catch (...) {
            AIDE_LOG_CRITICAL(
                "Could not create hierarchical menu Ids with static "
                "storage duration. This should only happen if the application "
                "is "
                "already out of memory at startup")
            std::terminate();
        }
    }
} // namespace aide::constants

#endif // AIDE_AIDE_CONSTANTS_HPP
