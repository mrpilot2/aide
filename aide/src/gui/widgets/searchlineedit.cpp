#include "searchlineedit.hpp"

#include "ui_searchlineedit.h"

using aide::SearchLineEdit;

SearchLineEdit::SearchLineEdit(QWidget* parent)
    : QWidget(parent)
    , m_ui{new Ui::SearchLineEdit}
{
    m_ui->setupUi(this);
}
SearchLineEdit::~SearchLineEdit() = default;

void SearchLineEdit::setSearchHint(const std::string& searchHint)
{
    m_ui->searchHint->setText(QString::fromStdString(searchHint));
}

void SearchLineEdit::setSearchIcon(const QIcon& icon)
{
    m_ui->searchIcon->setPixmap(icon.pixmap(16, 16));
}
