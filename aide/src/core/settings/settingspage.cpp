#include "aide/settings/settingspage.hpp"

#include <algorithm>

#include <QAbstractButton>
#include <QGraphicsOpacityEffect>
#include <QGroupBox>
#include <QLabel>
#include <QString>
#include <QWidget>

#include "settings/searchpattern.hpp"

using aide::HierarchicalId;
using aide::core::SettingsPage;

namespace
{
    constexpr auto* highlightStyleSheet = "border: 2px solid #8B4513;";
    constexpr auto dimmedOpacity        = 0.35;

    void applyDimming(QWidget* widget, bool dim)
    {
        if (dim) {
            auto* effect = new QGraphicsOpacityEffect(widget);
            effect->setOpacity(dimmedOpacity);
            widget->setGraphicsEffect(effect);
        } else {
            widget->setGraphicsEffect(nullptr);
        }
    }

    template <typename WidgetType, typename TextAccessor>
    void highlightMatchingWidgets(QWidget* page, const QString& pattern,
                                  TextAccessor textOf)
    {
        const bool clear = pattern.isEmpty();

        const auto widgets = page->findChildren<WidgetType*>();
        for (auto* child : widgets) {
            const bool match =
                !clear && textOf(child).contains(pattern, Qt::CaseInsensitive);
            child->setStyleSheet(match ? highlightStyleSheet : "");
        }
    }

    /**
     * @brief Whether a widget bears text that matches the pattern.
     *
     * Only the widget types that carry searchable text and therefore receive
     * the highlight border participate: labels, buttons/check boxes and group
     * boxes. Every other widget is treated as non-matching content, consistent
     * with highlightMatchingWidgets().
     */
    bool isHighlightedContent(const QWidget* widget, const QString& pattern)
    {
        if (const auto* label = qobject_cast<const QLabel*>(widget)) {
            return label->text().contains(pattern, Qt::CaseInsensitive);
        }
        if (const auto* button = qobject_cast<const QAbstractButton*>(widget)) {
            return button->text().contains(pattern, Qt::CaseInsensitive);
        }
        if (const auto* groupBox = qobject_cast<const QGroupBox*>(widget)) {
            return groupBox->title().contains(pattern, Qt::CaseInsensitive);
        }
        return false;
    }

    bool subtreeContainsHighlight(const QWidget* widget, const QString& pattern)
    {
        if (isHighlightedContent(widget, pattern)) { return true; }

        const auto children =
            widget->findChildren<QWidget*>(Qt::FindDirectChildrenOnly);
        return std::ranges::any_of(children, [&pattern](const QWidget* child) {
            return subtreeContainsHighlight(child, pattern);
        });
    }

    /**
     * @brief Dims every widget that is not a match or an ancestor of one.
     *
     * A subtree that contains a match is descended into so the match and its
     * ancestors stay at full opacity while their non-matching siblings are
     * dimmed. A subtree with no match at all is dimmed as a whole through a
     * single opacity effect on its root, which also covers a compound
     * control's internal widgets (e.g. a spin box's embedded line edit)
     * without stacking effects. This is deliberately type-independent so that
     * any standard Qt widget on a client-provided page is handled.
     */
    void grayOutNonMatching(QWidget* widget, const QString& pattern)
    {
        if (subtreeContainsHighlight(widget, pattern)) {
            const auto children =
                widget->findChildren<QWidget*>(Qt::FindDirectChildrenOnly);
            for (auto* child : children) {
                grayOutNonMatching(child, pattern);
            }
            return;
        }
        applyDimming(widget, true);
    }
} // namespace

SettingsPage::SettingsPage(HierarchicalId group)
    : settingsGroup{std::move(group)}
{}

SettingsPage::~SettingsPage() = default;

const HierarchicalId& SettingsPage::group() const
{
    return settingsGroup;
}

bool SettingsPage::matches(const QString& pattern)
{
    QWidget* pageWidget = widget();
    if (pageWidget == nullptr) { return false; }

    const auto words = tokenizeSearchPattern(pattern);

    const auto contains = [&words](const QString& text) {
        return matchesAnyWord(text, words);
    };

    const auto labels = pageWidget->findChildren<QLabel*>();
    if (std::ranges::any_of(labels, [&](const QLabel* label) {
            return contains(label->text());
        })) {
        return true;
    }

    const auto buttons = pageWidget->findChildren<QAbstractButton*>();
    if (std::ranges::any_of(buttons, [&](const QAbstractButton* button) {
            return contains(button->text());
        })) {
        return true;
    }

    const auto groupBoxes = pageWidget->findChildren<QGroupBox*>();
    return std::ranges::any_of(groupBoxes, [&](const QGroupBox* groupBox) {
        return contains(groupBox->title());
    });
}

void SettingsPage::highlight(const QString& pattern)
{
    QWidget* pageWidget = widget();
    if (pageWidget == nullptr) { return; }

    highlightMatchingWidgets<QLabel>(
        pageWidget, pattern, [](const QLabel* label) { return label->text(); });
    highlightMatchingWidgets<QAbstractButton>(
        pageWidget, pattern,
        [](const QAbstractButton* button) { return button->text(); });
    highlightMatchingWidgets<QGroupBox>(
        pageWidget, pattern,
        [](const QGroupBox* groupBox) { return groupBox->title(); });

    // Clear any dimming from a previous search before recomputing it.
    const auto allWidgets = pageWidget->findChildren<QWidget*>();
    for (auto* child : allWidgets) {
        child->setGraphicsEffect(nullptr);
    }
    if (pattern.isEmpty()) { return; }

    const auto topLevel =
        pageWidget->findChildren<QWidget*>(Qt::FindDirectChildrenOnly);
    for (auto* child : topLevel) {
        grayOutNonMatching(child, pattern);
    }
}
