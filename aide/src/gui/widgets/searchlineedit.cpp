#include "widgets/searchlineedit.hpp"

#include <QIcon>
#include <QMenu>
#include <QObject>
#include <QTimer>

#include "multicolumnsortfilterproxymodel.hpp"
#include "ui_searchlineedit.h"

using aide::widgets::SearchLineEdit;

SearchLineEdit::SearchLineEdit(QWidget* parent)
    : QWidget(parent)
    , m_ui{new Ui::SearchLineEdit}
    , m_typingTimer{new QTimer{this}}
    , m_filterModel(new MultiColumnSortFilterProxyModel(this))
{
    m_ui->setupUi(this);

    connect(m_ui->searchField, &QLineEdit::textChanged, this,
            &SearchLineEdit::textChanged);

    m_typingTimer->setSingleShot(true);
    connect(m_typingTimer, &QTimer::timeout, this,
            &SearchLineEdit::filterEntries);
    connect(m_ui->matchCase, &QCheckBox::stateChanged, this,
            &SearchLineEdit::filterEntries);
    connect(m_ui->regularExpression, &QCheckBox::stateChanged, this,
            &SearchLineEdit::filterEntries);
}

SearchLineEdit::~SearchLineEdit() = default;

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

void SearchLineEdit::filterEntries()
{
    m_filterModel->setFilterCaseSensitivity(
        m_ui->matchCase->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive);
    m_filterModel->setFilterOption(m_ui->regularExpression->isChecked()
                                       ? FilterOption::Regex
                                       : FilterOption::Wildcard);
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
