#ifndef AIDE_GOTIT_TOOLTIP_HPP
#define AIDE_GOTIT_TOOLTIP_HPP

#include <functional>
#include <string>

#include <QColor>
#include <QPointer>
#include <QString>
#include <QWidget>

#include <aide/gui/widgets/aidewidgets_export.h>

class QLabel;
class QPushButton;
class QToolButton;

namespace aide
{
    class SettingsInterface;
} // namespace aide

namespace aide::widgets
{
    enum class GotItPosition
    {
        Below,
        Above,
        Left,
        Right
    };

    /**
     * @brief One-shot onboarding tooltip anchored to a target widget, arrow
     * pointing at it. Caller-driven: never post()s, never logged (see #145),
     * and has no NotificationManager dependency — only SettingsInterface and
     * the target QWidget (see #147).
     *
     * Seen state is an int show-count at SettingsInterface key
     * "aide/gotit/<id>" (canShow() == count < maxCount, default 1);
     * withShowCount() raises maxCount. Reshow a changed tip by bumping @p id
     * (the ".v2" convention — a fresh key starts at 0). @p id is copied into
     * this tooltip, but pass a string literal by convention: it is also
     * threaded through HierarchicalId at each settings access, which stores
     * a raw pointer rather than copying, matching every other
     * HierarchicalId call site in aIDE.
     *
     * Only one tooltip shows at a time; closing one advances a single
     * process-wide queue (kept file-local in gotittooltip.cpp).
     */
    class AIDEWIDGETS_EXPORT GotItTooltip : public QWidget
    {
        Q_OBJECT
    public:
        GotItTooltip(aide::SettingsInterface& settings, const char* id,
                     const QString& text, QWidget* parent = nullptr);

        ~GotItTooltip() override;

        GotItTooltip(const GotItTooltip&)            = delete;
        GotItTooltip& operator=(const GotItTooltip&) = delete;
        GotItTooltip(GotItTooltip&&)                 = delete;
        GotItTooltip& operator=(GotItTooltip&&)      = delete;

        GotItTooltip& withHeader(const QString& header);
        GotItTooltip& withStep(int step);
        GotItTooltip& withLink(const QString& text,
                               std::function<void()> handler);
        GotItTooltip& withShowCount(int maxCount);
        GotItTooltip& withTimeout(int timeoutMs);

        [[nodiscard]] bool canShow() const;

        /**
         * @brief Shows this tooltip anchored to @p target, deferring until
         * it becomes visible with non-empty bounds. No-op (and deletes this)
         * if canShow() is false or @p target is null.
         */
        void showGotIt(QWidget* target,
                       GotItPosition position = GotItPosition::Below);

        /**
         * @brief Compares SettingsInterface key "aide/gotit/previous/run"
         * against QCoreApplication::applicationVersion(), stores the current
         * version, and returns whether they differed — i.e. whether this is
         * the first run after an upgrade.
         */
        static bool isFirstRunAfterUpgrade(aide::SettingsInterface& settings);

    signals:
        void closed();

    protected:
        void paintEvent(QPaintEvent* event) override;
        bool eventFilter(QObject* watched, QEvent* event) override;

    private:
        void enqueue();
        void waitForTargetThenShow();
        void showNow();
        void dismiss();
        void advanceQueue();
        void positionNearTarget();
        void applyArrowMargins();
        [[nodiscard]] int showCount() const;
        void recordShown();

        aide::SettingsInterface& m_settings;
        std::string m_id;
        int m_maxCount{1};
        int m_timeoutMs{0};

        QPointer<QWidget> m_target;
        GotItPosition m_position{GotItPosition::Below};
        bool m_shown{false};
        QColor m_surfaceColor;

        QLabel* m_stepLabel{nullptr};
        QLabel* m_headerLabel{nullptr};
        QLabel* m_bodyLabel{nullptr};
        QToolButton* m_linkButton{nullptr};
        QPushButton* m_gotItButton{nullptr};

        std::function<void()> m_linkHandler;
    };
} // namespace aide::widgets

#endif // AIDE_GOTIT_TOOLTIP_HPP
