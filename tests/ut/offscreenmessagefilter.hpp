#ifndef AIDE_OFFSCREEN_MESSAGE_FILTER_HPP
#define AIDE_OFFSCREEN_MESSAGE_FILTER_HPP

#include <QByteArray>
#include <QString>
#include <QtGlobal>

namespace aide::test
{
    namespace detail
    {
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
        inline QtMessageHandler previousMessageHandler{nullptr};

        inline void dropOffscreenUnsupportedFeatureWarnings(
            QtMsgType type, const QMessageLogContext& context,
            const QString& message)
        {
            // QPlatformWindow's default implementations of
            // propagateSizeHints(), raise(), setKeyboardGrabEnabled(), ...
            // all warn "This plugin does not support <feature>". The
            // offscreen plugin leaves them unimplemented, so every widget
            // test that shows a window prints them although nothing is
            // wrong with the code under test.
            if (type == QtWarningMsg && message.startsWith(QStringLiteral(
                                            "This plugin does not support"))) {
                return;
            }
            if (previousMessageHandler != nullptr) {
                previousMessageHandler(type, context, message);
            }
        }
    } // namespace detail

    // Silences the offscreen QPA plugin's "This plugin does not support ..."
    // warnings, and nothing else: every other Qt message - including warnings
    // that point at real problems in the code under test - still reaches the
    // default handler. Only active when the tests actually run offscreen, so
    // an explicit QT_QPA_PLATFORM override keeps the full platform output.
    inline void installOffscreenMessageFilter()
    {
        if (qgetenv("QT_QPA_PLATFORM") != QByteArray("offscreen")) { return; }
        detail::previousMessageHandler = qInstallMessageHandler(
            detail::dropOffscreenUnsupportedFeatureWarnings);
    }
} // namespace aide::test

#endif // AIDE_OFFSCREEN_MESSAGE_FILTER_HPP
