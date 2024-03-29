#include "widgets/searchfilterwidget.hpp"

#include <iostream>

#include <QLineEdit>
#include <QMenu>
#include <QObject>
#include <QTimer>

#include <aide/aidesettingsprovider.hpp>
#include <aide/settingsinterface.hpp>

#include "multicolumnsortfilterproxymodel.hpp"
#include "searchfilterresultdelegate.hpp"
#include "ui_searchfilterwidget.h"

using aide::widgets::SearchFilterWidget;

SearchFilterWidget::SearchFilterWidget(const HierarchicalId& id,
                                       const QKeySequence& showHideShortcut,
                                       QWidget* parent)
    : QWidget(parent)
    , m_ui{std::make_unique<Ui::SearchFilterWidget>()}
    , m_typingTimer{new QTimer{this}}
    , m_filterModel(new MultiColumnSortFilterProxyModel(this))
    , m_showHideAction(this)
    , m_visibilitySettingsKey{id.addLevel("isVisible")}
    , m_matchCaseSettingsKey{id.addLevel("matchCase")}
    , m_regexSettingsKey{id.addLevel("regex")}
{
    m_ui->setupUi(this);

    const auto guiSettings = AideSettingsProvider::versionableSettings();

    bool visibilitySetting{true};

    if (guiSettings != nullptr) {
        visibilitySetting =
            guiSettings->value(m_visibilitySettingsKey).toBool();

        m_ui->matchCase->setChecked(
            guiSettings->value(m_matchCaseSettingsKey, false).toBool());
        m_ui->regularExpression->setChecked(
            guiSettings->value(m_regexSettingsKey, true).toBool());
        matchCaseStateChanged(m_ui->matchCase->isChecked());
        regexStateChanged(m_ui->regularExpression->isChecked());
    }

    connect(m_ui->searchField, &QLineEdit::textChanged, this,
            &SearchFilterWidget::textChanged);

    m_typingTimer->setSingleShot(true);
    connect(m_typingTimer, &QTimer::timeout, this,
            &SearchFilterWidget::filterEntries);
    connect(m_ui->matchCase, &QCheckBox::toggled, this,
            &SearchFilterWidget::matchCaseStateChanged);
    connect(m_ui->regularExpression, &QCheckBox::toggled, this,
            &SearchFilterWidget::regexStateChanged);

    m_showHideAction.setShortcut(showHideShortcut);
    m_showHideAction.setCheckable(true);
    m_showHideAction.setChecked(this->isVisible());

    if (parent != nullptr) {
        parent->addAction(&m_showHideAction);
    } else {
        std::cout
            << "To make the show/hide action of the SearchFilterWidget "
               "available, "
               "a "
               "parent needs to be set. The show/hide functionality is now "
               "disabled.\n";
        visibilitySetting = true;
    }

    connect(&m_showHideAction, &QAction::toggled, this,
            &SearchFilterWidget::onUserRequestsToChangeVisibility);

#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
    m_ui->regularExpression->setChecked(true);
    m_ui->regularExpression->hide();
    regexStateChanged(m_ui->regularExpression->isChecked());
#endif

    // Set visibility delayed - overridden functions shall not be called
    // in constructor
    // NOLINTNEXTLINE
    QTimer::singleShot(VISIBILITY_TOGGLE_DELAY, [this, visibilitySetting]() {
        this->setVisible(visibilitySetting);
    });
}

SearchFilterWidget::~SearchFilterWidget() = default;

void SearchFilterWidget::onUserRequestsToChangeVisibility(const bool visible)
{
    this->setVisible(visible);

    if (const auto guiSettings = AideSettingsProvider::versionableSettings();
        guiSettings != nullptr) {
        guiSettings->setValue(m_visibilitySettingsKey, visible);
    }
}

void SearchFilterWidget::setSearchIcon(const QIcon& icon) const
{
    m_ui->toolButton->setIcon(icon);
}

void SearchFilterWidget::setSourceModel(QAbstractItemModel* model) const
{
    m_filterModel->setSourceModel(model);
}

QSortFilterProxyModel* SearchFilterWidget::getFilterModel() const
{
    return m_filterModel;
}

QAbstractItemDelegate* SearchFilterWidget::getItemDelegate()
{
    return new SearchFilterResultDelegate(this);
}

void SearchFilterWidget::textChanged(const QString& text)
{
    m_currentFilterText = text;
    m_typingTimer->start(TYPING_FILTER_DELAY);
}

void SearchFilterWidget::matchCaseStateChanged(const bool state) const
{
    m_filterModel->setFilterCaseSensitivity(
        m_ui->matchCase->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);

    if (const auto guiSettings = AideSettingsProvider::versionableSettings();
        guiSettings != nullptr) {
        guiSettings->setValue(m_matchCaseSettingsKey, state);

        filterEntries();
    }
}

void SearchFilterWidget::regexStateChanged(const bool state) const
{
    m_filterModel->setFilterOption(m_ui->regularExpression->isChecked()
                                       ? FilterOption::Regex
                                       : FilterOption::Wildcard);
    if (const auto guiSettings = AideSettingsProvider::versionableSettings();
        guiSettings != nullptr) {
        guiSettings->setValue(m_regexSettingsKey, state);

        filterEntries();
    }
}

void SearchFilterWidget::filterEntries() const
{
    if (m_filterModel == nullptr || m_filterModel->sourceModel() == nullptr) {
        return;
    }

    QList<QString> tags;
    m_filterModel->clearFilterForAllColumns();

    auto parts = m_currentFilterText.trimmed().split(' ');
    for (int part_index = 0; part_index < parts.size(); ++part_index) {
        if (parts[part_index].endsWith(':')) {
            auto possibleColumnName = parts[part_index];

            possibleColumnName =
                possibleColumnName.left(possibleColumnName.lastIndexOf(':'))
                    .toLower();

            bool found = false;
            int index  = -1;
            for (int i = 0; i < m_filterModel->sourceModel()->columnCount();
                 ++i) {
                if (m_filterModel->sourceModel()
                        ->headerData(i, Qt::Horizontal)
                        .toString()
                        .toLower() == possibleColumnName) {
                    tags.append(possibleColumnName + ':');
                    found = true;
                    index = i;
                    break;
                }
            }

            if (found && part_index < parts.size() - 1 &&
                !parts[part_index + 1].trimmed().isEmpty()) {
                m_filterModel->setFilterForColumn(index, parts[part_index + 1]);
                part_index++;
            }
        } else {
            m_filterModel->setFilterForColumn(-1, parts[part_index]);
        }
    }

    m_ui->searchField->setTags(tags);

    if (m_filterModel->rowCount() < m_filterModel->sourceModel()->rowCount()) {
        m_ui->filterResult->setText(
            QString("Found %1 matches out of %2 elements")
                .arg(
                    QString::number(m_filterModel->rowCount()),
                    QString::number(m_filterModel->sourceModel()->rowCount())));
    } else {
        m_ui->filterResult->setText("");
    }
}
