#include "widgets/searchlineedit.hpp"

#include <QDebug>
#include <QIcon>
#include <QMenu>
#include <QObject>
#include <QTimer>

#include <aide/aidesettingsprovider.hpp>
#include <aide/settingsinterface.hpp>

#include "multicolumnsortfilterproxymodel.hpp"
#include "ui_searchlineedit.h"

using aide::widgets::SearchLineEdit;

SearchLineEdit::SearchLineEdit(const aide::HierarchicalId& id,
                               const QKeySequence& showHideShortcut,
                               QWidget* parent)
    : QWidget(parent)
    , m_ui{new Ui::SearchLineEdit}
    , m_typingTimer{new QTimer{this}}
    , m_filterModel(new MultiColumnSortFilterProxyModel(this))
    , m_visibilitySettingsKey{id.addLevel("isVisible")}
    , m_matchCaseSettingsKey{id.addLevel("matchCase")}
    , m_regexSettingsKey{id.addLevel("regex")}
{
    m_ui->setupUi(this);

    auto guiSettings = aide::AideSettingsProvider::versionableSettings();

    bool visibilitySetting{
        guiSettings->value(m_visibilitySettingsKey).toBool()};

    m_ui->matchCase->setChecked(
        guiSettings->value(m_matchCaseSettingsKey, false).toBool());
    m_ui->regularExpression->setChecked(
        guiSettings->value(m_regexSettingsKey, true).toBool());
    matchCaseStateChanged(m_ui->matchCase->isChecked());
    regexStateChanged(m_ui->regularExpression->isChecked());

    connect(m_ui->searchField, &QLineEdit::textChanged, this,
            &SearchLineEdit::textChanged);

    m_typingTimer->setSingleShot(true);
    connect(m_typingTimer, &QTimer::timeout, this,
            &SearchLineEdit::filterEntries);
    connect(m_ui->matchCase, &QCheckBox::toggled, this,
            &SearchLineEdit::matchCaseStateChanged);
    connect(m_ui->regularExpression, &QCheckBox::toggled, this,
            &SearchLineEdit::regexStateChanged);

    auto* showHideAction = new QAction(this);
    showHideAction->setShortcut(showHideShortcut);
    showHideAction->setCheckable(true);
    showHideAction->setChecked(this->isVisible());

    if (parent != nullptr) {
        parent->addAction(showHideAction);
    } else {
        qDebug()
            << "To make the show/hide action of the SearchLineEdit available, "
               "a "
               "parent needs to be set. The show/hide functionality is now "
               "disabled.";
        visibilitySetting = true;
    }

    connect(showHideAction, &QAction::toggled, this,
            &SearchLineEdit::onUserRequestsToChangeVisibility);

#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
    m_ui->regularExpression->setChecked(true);
    m_ui->regularExpression->hide();
    regexStateChanged(m_ui->regularExpression->isChecked());
#endif

    // Set visibility delayed - overridden functions shall not be called
    // in constructor
    QTimer::singleShot(200, [this, visibilitySetting]() {
        this->setVisible(visibilitySetting);
    });
}

SearchLineEdit::~SearchLineEdit() = default;

void SearchLineEdit::onUserRequestsToChangeVisibility(bool visible)
{
    this->setVisible(visible);

    auto guiSettings = aide::AideSettingsProvider::versionableSettings();

    guiSettings->setValue(m_visibilitySettingsKey, visible);
}

void SearchLineEdit::setSearchIcon(const QIcon& icon)
{
    m_ui->toolButton->setIcon(icon);
}

void SearchLineEdit::setSourceModel(QAbstractItemModel* model)
{
    m_filterModel->setSourceModel(model);
}

QSortFilterProxyModel* SearchLineEdit::getFilterModel()
{
    return m_filterModel;
}

void SearchLineEdit::textChanged(const QString& text)
{
    m_currentFilterText = text;
    m_typingTimer->start(300);
}

void SearchLineEdit::matchCaseStateChanged(bool state)
{
    m_filterModel->setFilterCaseSensitivity(
        m_ui->matchCase->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);

    auto guiSettings = aide::AideSettingsProvider::versionableSettings();

    guiSettings->setValue(m_matchCaseSettingsKey, state);

    filterEntries();
}

void SearchLineEdit::regexStateChanged(bool state)
{
    m_filterModel->setFilterOption(m_ui->regularExpression->isChecked()
                                       ? FilterOption::Regex
                                       : FilterOption::Wildcard);

    auto guiSettings = aide::AideSettingsProvider::versionableSettings();

    guiSettings->setValue(m_regexSettingsKey, state);

    filterEntries();
}

void SearchLineEdit::filterEntries()
{
    if (m_filterModel == nullptr || m_filterModel->sourceModel() == nullptr) {
        return;
    }

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
