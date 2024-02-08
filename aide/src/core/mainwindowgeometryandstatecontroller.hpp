#ifndef AIDE_MAIN_WINDOW_GEOMETRY_AND_STATE_CONTROLLER_HPP
#define AIDE_MAIN_WINDOW_GEOMETRY_AND_STATE_CONTROLLER_HPP

class QByteArray;

namespace aide::core
{
    class MainWindowGeometryAndStateController
    {
    public:
        virtual ~MainWindowGeometryAndStateController() = default;

        virtual void saveGeometryAndState(const QByteArray& geometry,
                                          const QByteArray& state) = 0;

        virtual void restoreGeometryAndState() = 0;
    };
} // namespace aide::core

#endif // AIDE_MAIN_WINDOW_GEOMETRY_AND_STATE_CONTROLLER_HPP
