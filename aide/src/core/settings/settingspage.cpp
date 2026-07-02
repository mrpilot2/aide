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
            applyDimming(child, !clear && !match);
        }
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
}
