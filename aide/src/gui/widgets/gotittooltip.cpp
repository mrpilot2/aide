#include "gotittooltip.hpp"

#include <algorithm>

#include <QCoreApplication>
#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QList>
#include <QPainter>
#include <QPainterPath>
#include <QPolygon>
#include <QPushButton>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

#include "aide/hierarchicalid.hpp"
#include "aide/settingsinterface.hpp"

using aide::HierarchicalId;
using aide::SettingsInterface;
using aide::widgets::GotItPosition;
using aide::widgets::GotItTooltip;

namespace
{
    constexpr auto GOTIT_SETTINGS_ROOT  = "aide";
    constexpr auto GOTIT_SETTINGS_GROUP = "gotit";
    constexpr auto PREVIOUS_RUN_GROUP   = "previous";
    constexpr auto PREVIOUS_RUN_KEY     = "run";

    constexpr int TOOLTIP_WIDTH{268};
    constexpr int CONTENT_MARGIN{13};
    constexpr int LAYOUT_SPACING{8};
    constexpr int STEP_BADGE_SIZE{20};
    constexpr int ARROW_SIZE{7};
    constexpr int ARROW_OFFSET{20};
    constexpr int TARGET_GAP{10};
    constexpr int SCREEN_EDGE_MARGIN{8};
    constexpr int BODY_RADIUS{7};
    constexpr int DEFAULT_MAX_COUNT{1};

    // Below this QPalette::Window lightness (0-255), the active palette
    // counts as a dark theme.
    constexpr int DARK_THEME_LIGHTNESS_THRESHOLD{128};
    // QColor::lighter()/darker() factor: >100 lightens/darkens by that
    // percentage, so 130 gives a 30% shift - enough to read as a distinct
    // surface from the app background while staying within the theme.
    constexpr int SURFACE_SHIFT_PERCENT{130};

    // Mirrors NotificationBalloon's balloonSurfaceColor(): QPalette::ToolTip*
    // roles are frequently not honoured by the platform theme for a
    // custom-painted popup like this one, which left the body readable in
    // one theme and not the other. Deriving the surface from the app's own
    // Window color instead keeps it paired with the WindowText color the
    // labels actually draw with.
    QColor surfaceColorFor(const QPalette& appPalette)
    {
        const auto windowColor = appPalette.color(QPalette::Window);
        const bool isDarkTheme =
            windowColor.lightness() < DARK_THEME_LIGHTNESS_THRESHOLD;
        return isDarkTheme ? windowColor.lighter(SURFACE_SHIFT_PERCENT)
                           : windowColor.darker(SURFACE_SHIFT_PERCENT);
    }

    QColor readableTextColorFor(const QColor& surface)
    {
        return surface.lightness() < DARK_THEME_LIGHTNESS_THRESHOLD ? Qt::white
                                                                    : Qt::black;
    }

    HierarchicalId gotItKey(const std::string& id)
    {
        return HierarchicalId(GOTIT_SETTINGS_ROOT)(GOTIT_SETTINGS_GROUP)(
            id.c_str());
    }

    HierarchicalId previousRunKey()
    {
        return HierarchicalId(GOTIT_SETTINGS_ROOT)(GOTIT_SETTINGS_GROUP)(
            PREVIOUS_RUN_GROUP)(PREVIOUS_RUN_KEY);
    }

    struct ArrowGeometry
    {
        QMargins layoutMargins;
        QRect bodyRect;
        QPolygon arrowPolygon;
    };

    // The single source of truth for how each Position carves an arrow
    // gutter out of the widget's own rect: which edge gets the extra layout
    // margin, how the rounded-rect body is inset, and where the arrow
    // triangle sits. Shared by applyArrowMargins() and paintEvent() so the
    // four-way Position split exists exactly once.
    ArrowGeometry arrowGeometryFor(GotItPosition position,
                                   const QRect& widgetRect)
    {
        ArrowGeometry geometry;
        geometry.layoutMargins = QMargins(CONTENT_MARGIN, CONTENT_MARGIN,
                                          CONTENT_MARGIN, CONTENT_MARGIN);
        geometry.bodyRect      = widgetRect;

        switch (position) {
        case GotItPosition::Below:
            geometry.layoutMargins.setTop(CONTENT_MARGIN + ARROW_SIZE);
            geometry.bodyRect.adjust(0, ARROW_SIZE, 0, 0);
            geometry.arrowPolygon
                << QPoint(ARROW_OFFSET, ARROW_SIZE)
                << QPoint(ARROW_OFFSET + ARROW_SIZE, 0)
                << QPoint(ARROW_OFFSET + (2 * ARROW_SIZE), ARROW_SIZE);
            break;
        case GotItPosition::Above:
            geometry.layoutMargins.setBottom(CONTENT_MARGIN + ARROW_SIZE);
            geometry.bodyRect.adjust(0, 0, 0, -ARROW_SIZE);
            geometry.arrowPolygon
                << QPoint(ARROW_OFFSET, geometry.bodyRect.bottom())
                << QPoint(ARROW_OFFSET + ARROW_SIZE, widgetRect.bottom())
                << QPoint(ARROW_OFFSET + (2 * ARROW_SIZE),
                          geometry.bodyRect.bottom());
            break;
        case GotItPosition::Right:
            geometry.layoutMargins.setLeft(CONTENT_MARGIN + ARROW_SIZE);
            geometry.bodyRect.adjust(ARROW_SIZE, 0, 0, 0);
            geometry.arrowPolygon
                << QPoint(0, ARROW_OFFSET + ARROW_SIZE)
                << QPoint(ARROW_SIZE, ARROW_OFFSET)
                << QPoint(ARROW_SIZE, ARROW_OFFSET + (2 * ARROW_SIZE));
            break;
        case GotItPosition::Left:
            geometry.layoutMargins.setRight(CONTENT_MARGIN + ARROW_SIZE);
            geometry.bodyRect.adjust(0, 0, -ARROW_SIZE, 0);
            geometry.arrowPolygon
                << QPoint(widgetRect.right(), ARROW_OFFSET + ARROW_SIZE)
                << QPoint(geometry.bodyRect.right(), ARROW_OFFSET)
                << QPoint(geometry.bodyRect.right(),
                          ARROW_OFFSET + (2 * ARROW_SIZE));
            break;
        }
        return geometry;
    }

    // Single process-wide queue: only one GotItTooltip shows at a time,
    // closing one advances to the next (see #147). Deliberately file-local
    // rather than an injected instance, matching the "no manager
    // dependency" scope of #153. A function-local static (rather than a
    // namespace-scope global) keeps the mutable state out of the TU's
    // global namespace.
    struct GotItQueue
    {
        GotItTooltip* current{nullptr};
        QList<GotItTooltip*> pending;
    };

    GotItQueue& gotItQueue()
    {
        static GotItQueue queue;
        return queue;
    }
} // namespace

GotItTooltip::GotItTooltip(SettingsInterface& settings, const char* id,
                           const QString& text, QWidget* parent)
    : QWidget(parent)
    , m_settings(settings)
    , m_id(id)
    , m_maxCount(DEFAULT_MAX_COUNT)
    , m_stepLabel(new QLabel(this))
    , m_headerLabel(new QLabel(this))
    , m_bodyLabel(new QLabel(text, this))
    , m_linkButton(new QToolButton(this))
    , m_gotItButton(new QPushButton(tr("Got it"), this))
{
    // Stays a plain child widget (see NotificationBalloon's constructor for
    // why): showGotIt() reparents this to the target's window once known,
    // and a plain child positioned in that window's local coordinate space
    // needs no window flags and has no Wayland absolute-positioning issue.
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedWidth(TOOLTIP_WIDTH);

    m_surfaceColor          = surfaceColorFor(palette());
    QPalette contentPalette = palette();
    contentPalette.setColor(QPalette::WindowText,
                            readableTextColorFor(m_surfaceColor));
    setPalette(contentPalette);

    m_stepLabel->setFixedSize(STEP_BADGE_SIZE, STEP_BADGE_SIZE);
    m_stepLabel->setAlignment(Qt::AlignCenter);
    m_stepLabel->setAutoFillBackground(true);
    QPalette stepPalette = m_stepLabel->palette();
    stepPalette.setColor(QPalette::Window,
                         palette().color(QPalette::Highlight));
    stepPalette.setColor(QPalette::WindowText,
                         palette().color(QPalette::HighlightedText));
    m_stepLabel->setPalette(stepPalette);
    m_stepLabel->hide();

    QFont headerFont = m_headerLabel->font();
    headerFont.setBold(true);
    m_headerLabel->setFont(headerFont);
    m_headerLabel->setWordWrap(true);
    m_headerLabel->hide();

    m_bodyLabel->setWordWrap(true);

    m_linkButton->setAutoRaise(true);
    m_linkButton->setCursor(Qt::PointingHandCursor);
    m_linkButton->setStyleSheet(
        QStringLiteral("QToolButton { border: none; text-decoration: "
                       "underline; color: %1; }")
            .arg(palette().color(QPalette::Link).name()));
    m_linkButton->hide();
    connect(m_linkButton, &QToolButton::clicked, this, [this]() {
        if (m_linkHandler) { m_linkHandler(); }
        dismiss();
    });

    connect(m_gotItButton, &QPushButton::clicked, this, &GotItTooltip::dismiss);

    auto* headerRow = new QHBoxLayout;
    headerRow->setContentsMargins(0, 0, 0, 0);
    headerRow->setSpacing(LAYOUT_SPACING);
    headerRow->addWidget(m_stepLabel);
    headerRow->addWidget(m_headerLabel, 1);

    auto* footerRow = new QHBoxLayout;
    footerRow->setContentsMargins(0, 0, 0, 0);
    footerRow->setSpacing(LAYOUT_SPACING);
    footerRow->addWidget(m_linkButton);
    footerRow->addStretch(1);
    footerRow->addWidget(m_gotItButton);

    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(CONTENT_MARGIN, CONTENT_MARGIN,
                                   CONTENT_MARGIN, CONTENT_MARGIN);
    rootLayout->setSpacing(LAYOUT_SPACING);
    rootLayout->addLayout(headerRow);
    rootLayout->addWidget(m_bodyLabel);
    rootLayout->addLayout(footerRow);
}

GotItTooltip::~GotItTooltip()
{
    if (!m_target.isNull()) {
        m_target->removeEventFilter(this);
        if (auto* window = m_target->window();
            window != nullptr && window != m_target) {
            window->removeEventFilter(this);
        }
    }
    advanceQueue();
}

GotItTooltip& GotItTooltip::withHeader(const QString& header)
{
    m_headerLabel->setText(header);
    m_headerLabel->setVisible(!header.isEmpty());
    return *this;
}

GotItTooltip& GotItTooltip::withStep(int step)
{
    m_stepLabel->setText(QString::number(step));
    m_stepLabel->setVisible(true);
    return *this;
}

GotItTooltip& GotItTooltip::withLink(const QString& text,
                                     std::function<void()> handler)
{
    m_linkButton->setText(text);
    m_linkButton->setVisible(!text.isEmpty());
    m_linkHandler = std::move(handler);
    return *this;
}

GotItTooltip& GotItTooltip::withShowCount(int maxCount)
{
    m_maxCount = maxCount;
    return *this;
}

GotItTooltip& GotItTooltip::withTimeout(int timeoutMs)
{
    m_timeoutMs = timeoutMs;
    m_gotItButton->setVisible(false);
    return *this;
}

int GotItTooltip::showCount() const
{
    return m_settings.value(gotItKey(m_id), 0).toInt();
}

bool GotItTooltip::canShow() const
{
    return showCount() < m_maxCount;
}

void GotItTooltip::recordShown()
{
    m_settings.setValue(gotItKey(m_id), std::min(showCount() + 1, m_maxCount));
}

bool GotItTooltip::isFirstRunAfterUpgrade(SettingsInterface& settings)
{
    const auto currentVersion = QCoreApplication::applicationVersion();
    const auto storedVersion  = settings.value(previousRunKey());
    const bool firstRun =
        !storedVersion.isValid() || storedVersion.toString() != currentVersion;
    settings.setValue(previousRunKey(), currentVersion);
    return firstRun;
}

void GotItTooltip::showGotIt(QWidget* target, GotItPosition position)
{
    if (target == nullptr || !canShow()) {
        deleteLater();
        return;
    }

    m_target = target;
    // QWidget::window() is never null (a parentless widget is its own
    // window), so this always finds a real anchor. Plain child of the
    // target's own window: keeps this tooltip positioned, raised, and
    // minimized together with it.
    setParent(target->window());
    m_position = position;
    applyArrowMargins();
    enqueue();
}

void GotItTooltip::applyArrowMargins()
{
    layout()->setContentsMargins(
        arrowGeometryFor(m_position, rect()).layoutMargins);
}

void GotItTooltip::enqueue()
{
    auto& queue = gotItQueue();
    if (queue.current != nullptr) {
        queue.pending.append(this);
        return;
    }
    queue.current = this;
    waitForTargetThenShow();
}

void GotItTooltip::waitForTargetThenShow()
{
    if (m_target.isNull()) {
        advanceQueue();
        deleteLater();
        return;
    }

    // Filters stay installed for the tooltip's whole lifetime (removed only
    // in the destructor), not just until first shown: once visible, the same
    // window events keep the tooltip in step with its target's window - see
    // eventFilter().
    m_target->installEventFilter(this);
    if (auto* window = m_target->window();
        window != nullptr && window != m_target) {
        window->installEventFilter(this);
    }

    if (m_target->isVisible() && !m_target->rect().isEmpty()) { showNow(); }
}

bool GotItTooltip::eventFilter(QObject* watched, QEvent* event)
{
    if (m_target.isNull()) { return QWidget::eventFilter(watched, event); }

    if (!m_shown) {
        // Show fires once the target (or its window) becomes visible;
        // Resize covers the case named in #147 - target already shown but
        // still at empty bounds, e.g. a freshly-added toolbar button
        // awaiting layout.
        const bool relevantEvent =
            event->type() == QEvent::Show || event->type() == QEvent::Resize;
        if (relevantEvent && m_target->isVisible() &&
            !m_target->rect().isEmpty()) {
            showNow();
        }
        return QWidget::eventFilter(watched, event);
    }

    // Once shown, the target's window is the only thing still tracked: keep
    // the tooltip anchored while it moves or resizes, and hide/restore it in
    // step with minimize, mirroring NotificationBalloonHost's anchor window
    // handling.
    if (auto* window = m_target->window(); watched == window) {
        switch (event->type()) {
        case QEvent::Move:
        case QEvent::Resize:
            positionNearTarget();
            break;
        case QEvent::WindowStateChange:
        {
            const bool minimized = window->isMinimized();
            setVisible(!minimized);
            if (!minimized) { positionNearTarget(); }
            break;
        }
        default:
            break;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void GotItTooltip::showNow()
{
    m_shown = true;
    positionNearTarget();
    show();
    raise();

    if (m_timeoutMs > 0) {
        QTimer::singleShot(m_timeoutMs, this, &GotItTooltip::dismiss);
    }
}

void GotItTooltip::positionNearTarget()
{
    if (m_target.isNull()) { return; }

    auto* window = m_target->window();
    const QRect targetRect(m_target->mapTo(window, QPoint(0, 0)),
                           m_target->size());
    const int width = TOOLTIP_WIDTH;
    // sizeHint().height() alone under-measures the wrapped body label here:
    // it reflects the layout's current width, which may still be the
    // widget's pre-resize default rather than the fixed TOOLTIP_WIDTH,
    // giving a too-short box for the actual wrapped text. Asking the layout
    // for the height at the width we're about to set avoids that.
    const int height = layout()->hasHeightForWidth()
                           ? layout()->totalHeightForWidth(width)
                           : sizeHint().height();
    resize(width, height);

    int posX = targetRect.left();
    int posY = targetRect.top();

    switch (m_position) {
    case GotItPosition::Below:
        posX = targetRect.left();
        posY = targetRect.bottom() + TARGET_GAP;
        break;
    case GotItPosition::Above:
        posX = targetRect.left();
        posY = targetRect.top() - height - TARGET_GAP;
        break;
    case GotItPosition::Right:
        posX = targetRect.right() + TARGET_GAP;
        posY = targetRect.top();
        break;
    case GotItPosition::Left:
        posX = targetRect.left() - width - TARGET_GAP;
        posY = targetRect.top();
        break;
    }

    const QRect avail = window->rect();
    posX              = std::clamp(posX, avail.left() + SCREEN_EDGE_MARGIN,
                                   std::max(avail.left() + SCREEN_EDGE_MARGIN,
                                            avail.right() - width - SCREEN_EDGE_MARGIN));
    posY              = std::clamp(posY, avail.top() + SCREEN_EDGE_MARGIN,
                                   std::max(avail.top() + SCREEN_EDGE_MARGIN,
                                            avail.bottom() - height - SCREEN_EDGE_MARGIN));

    move(posX, posY);
}

void GotItTooltip::dismiss()
{
    recordShown();
    hide();
    emit closed();
    advanceQueue();
    deleteLater();
}

void GotItTooltip::advanceQueue()
{
    auto& queue = gotItQueue();
    queue.pending.removeAll(this);
    if (queue.current != this) { return; }

    queue.current = nullptr;
    if (!queue.pending.isEmpty()) {
        auto* next    = queue.pending.takeFirst();
        queue.current = next;
        next->waitForTargetThenShow();
    }
}

void GotItTooltip::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const auto geometry = arrowGeometryFor(m_position, rect());

    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRoundedRect(geometry.bodyRect, BODY_RADIUS, BODY_RADIUS);
    path.addPolygon(geometry.arrowPolygon);

    painter.fillPath(path, m_surfaceColor);
    painter.setPen(palette().color(QPalette::Mid));
    painter.drawPath(path);
}
