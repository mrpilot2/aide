
#ifndef AIDE_APPLICATION_CONFIG_HPP
#define AIDE_APPLICATION_CONFIG_HPP

#include <memory>

#include <QUrl>

#include <aide/helptopicid.hpp>
#include <aide/urllauncherinterface.hpp>

namespace aide
{
    /**
     * @brief Consumer-facing set of built-in feature toggles for aIDE.
     *
     * A client fills in an ApplicationConfig programmatically and hands it to
     * the aide::Application at construction time to declare which built-in
     * features the application uses. Every feature defaults to enabled, so a
     * zero-configuration application behaves exactly like today's aIDE - with
     * two documented exceptions, ShowLogInFileManagerAction and
     * ReportBugAction, which default to disabled since they are only
     * appropriate for developer-facing consumer applications and would
     * otherwise expose concepts (the log file, aIDE's own issue tracker)
     * that end users of most aIDE-based applications have no reason to know
     * about.
     *
     * The type is an ordinary copyable value with a private implementation
     * (pImpl) so its binary layout stays stable as new toggles are added:
     * growing the configuration is an additive enum value plus a default-table
     * entry, never a public layout change. Copies deep-copy their overrides, so
     * a copy can be modified without affecting the original.
     */
    class ApplicationConfig
    {
    public:
        /**
         * @brief Built-in features that a consumer can switch on or off.
         *
         * New features are appended as additional enum values; each also gets a
         * default-table entry in the implementation.
         */
        enum class Feature
        {
            /// Checkable View → Full Screen action bound to F11.
            ViewFullscreenAction,

            /// Help → "Show Log in <File Manager>" action. Defaults to
            /// disabled, unlike most other features in this enum.
            ShowLogInFileManagerAction,

            /// Help → "Report Bug in aIDE" action. Defaults to disabled,
            /// unlike most other features in this enum.
            ReportBugAction,

            /// The web-based help system. Help is only active when this is
            /// enabled *and* a Help Base URL is set (see setHelpBaseUrl()).
            Help,
        };

        ApplicationConfig();
        ~ApplicationConfig();

        ApplicationConfig(const ApplicationConfig& other);
        ApplicationConfig& operator=(const ApplicationConfig& other);
        ApplicationConfig(ApplicationConfig&& other) noexcept;
        ApplicationConfig& operator=(ApplicationConfig&& other) noexcept;

        /**
         * @brief Enables or disables a feature.
         * @return *this, so calls can be chained fluently.
         */
        ApplicationConfig& setEnabled(Feature feature, bool enabled);

        /**
         * @brief Whether a feature is enabled.
         * @return the consumer's override if one was set, otherwise the
         *         feature's built-in default.
         */
        [[nodiscard]] bool isEnabled(Feature feature) const;

        /**
         * @brief Substitutes a consumer-supplied launcher for the one
         * used to open URLs (e.g. by the "Report Bug in aIDE" action),
         * instead of the default OS-backed launcher.
         * @return *this, so calls can be chained fluently.
         */
        ApplicationConfig& setUrlLauncher(UrlLauncherPtr launcher);

        /**
         * @brief The consumer-supplied URL launcher override, if any.
         * @return the overriding launcher, or nullptr if none was set - in
         *         which case the caller should fall back to the default
         *         OS-backed launcher.
         */
        [[nodiscard]] UrlLauncherPtr urlLauncher() const;

        /**
         * @brief Sets the Help Base URL, the root of the client's web help.
         *
         * A Help Topic resolves to `<base>/<topic-id>/`; a missing trailing
         * slash on the base is added. Without a base URL help stays inactive
         * even when Feature::Help is enabled.
         * @return *this, so calls can be chained fluently.
         */
        ApplicationConfig& setHelpBaseUrl(const QUrl& baseUrl);

        /**
         * @brief The Help Base URL, or an empty QUrl if none was set.
         */
        [[nodiscard]] QUrl helpBaseUrl() const;

        /**
         * @brief Sends the Help Topic @p source to the location of @p target.
         *
         * Remaps apply one step only (`a→b`, `b→c` sends `a` to `b`), and
         * the last registration for @p source wins. Remapping onto a disabled
         * topic does not disable @p source.
         * @return *this, so calls can be chained fluently.
         */
        ApplicationConfig& remapHelpTopic(const HelpTopicId& source,
                                          const HelpTopicId& target);

        /**
         * @brief Where the Help Topic @p id resolves to after remapping.
         * @return the remap target if @p id was remapped, otherwise @p id.
         */
        [[nodiscard]] HelpTopicId remappedHelpTopic(
            const HelpTopicId& id) const;

        /**
         * @brief Turns off help for the Help Topic @p id.
         *
         * Checked against the declared ID, before any remap.
         * @return *this, so calls can be chained fluently.
         */
        ApplicationConfig& disableHelpTopic(const HelpTopicId& id);

        /**
         * @brief Whether disableHelpTopic() was called for @p id.
         */
        [[nodiscard]] bool isHelpTopicDisabled(const HelpTopicId& id) const;

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };
} // namespace aide

#endif // AIDE_APPLICATION_CONFIG_HPP
