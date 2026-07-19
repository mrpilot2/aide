#include "gotittooltip.hpp"

#include <algorithm>

#include <QCoreApplication>
#include <QEvent>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include <QPainter>
#include <QPainterPath>
#include <QPolygon>
#include <QPushButton>
#include <QScreen>
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
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool |
                   Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedWidth(TOOLTIP_WIDTH);

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

    m_target   = target;
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

    if (m_target->isVisible() && !m_target->rect().isEmpty()) {
        showNow();
        return;
    }

    m_target->installEventFilter(this);
    if (auto* window = m_target->window();
        window != nullptr && window != m_target) {
        window->installEventFilter(this);
    }
}

bool GotItTooltip::eventFilter(QObject* watched, QEvent* event)
{
    // Show fires once the target (or its window) becomes visible; Resize
    // covers the case named in #147 - target already shown but still at
    // empty bounds, e.g. a freshly-added toolbar button awaiting layout.
    const bool relevantEvent =
        event->type() == QEvent::Show || event->type() == QEvent::Resize;
    if (relevantEvent && !m_target.isNull() && m_target->isVisible() &&
        !m_target->rect().isEmpty()) {
        m_target->removeEventFilter(this);
        if (auto* window = m_target->window();
            window != nullptr && window != m_target) {
            window->removeEventFilter(this);
        }
        showNow();
    }
    return QWidget::eventFilter(watched, event);
}

void GotItTooltip::showNow()
{
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

    const QRect targetRect(m_target->mapToGlobal(QPoint(0, 0)),
                           m_target->size());
    const int width  = TOOLTIP_WIDTH;
    const int height = sizeHint().height();
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

    if (auto* screen = m_target->screen(); screen != nullptr) {
        const auto avail = screen->availableGeometry();
        posX             = std::clamp(posX, avail.left() + SCREEN_EDGE_MARGIN,
                                      std::max(avail.left() + SCREEN_EDGE_MARGIN,
                                               avail.right() - width - SCREEN_EDGE_MARGIN));
        posY =
            std::clamp(posY, avail.top() + SCREEN_EDGE_MARGIN,
                       std::max(avail.top() + SCREEN_EDGE_MARGIN,
                                avail.bottom() - height - SCREEN_EDGE_MARGIN));
    }

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

    painter.fillPath(path, palette().color(QPalette::ToolTipBase));
    painter.setPen(palette().color(QPalette::Mid));
    painter.drawPath(path);
}
