#ifndef DEMO_COLOR_SCHEME_REACTOR_HPP
#define DEMO_COLOR_SCHEME_REACTOR_HPP

#include <QObject>
#include <QString>

#include <aide/appearancemanager.hpp>
#include <aide/colorscheme.hpp>

namespace demo
{
    inline QString iconPathForScheme(aide::ColorScheme scheme)
    {
        return scheme == aide::ColorScheme::Light ? ":/demo/icons/dark"
                                                  : ":/demo/icons/light";
    }

    /**
     * @brief Updates the demo's System-theme icon search path when the OS
     * color scheme changes.
     *
     * A dedicated QObject slot is used instead of a lambda so the connection
     * can be made with the string-based SIGNAL/SLOT syntax. The
     * pointer-to-member syntax would reference
     * AppearanceManager::staticMetaObject, a data symbol that
     * CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS does not export across DLL boundaries
     * on MSVC shared builds. The string form resolves the signal at runtime
     * via the exported virtual metaObject().
     */
    class ColorSchemeReactor : public QObject
    {
        Q_OBJECT
    public:
        explicit ColorSchemeReactor(aide::AppearanceManager& manager,
                                    QObject* parent = nullptr)
            : QObject(parent)
            , m_manager(manager)
        {}

    public slots:
        void onColorSchemeChanged(aide::ColorScheme scheme)
        {
            m_manager.addIconSearchPath("System", iconPathForScheme(scheme));
        }

    private:
        aide::AppearanceManager& m_manager;
    };
} // namespace demo

#endif // DEMO_COLOR_SCHEME_REACTOR_HPP
