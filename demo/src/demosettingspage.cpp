#include "demosettingspage.hpp"

#include <utility>

#include <QPushButton>
#include <QWidget>

#include <aide/hierarchicalid.hpp>

#include "ui_demosettingspage.h"

using aide::HierarchicalId;
using aide::core::SettingsPage;

DemoSettingsPage::DemoSettingsPage(aide::HierarchicalId group)
    : SettingsPage(std::move(group))
    , m_ui{std::make_unique<Ui::DemoSettingsPage>()}
    , m_widget(nullptr)
    , m_tableInsertTimer{std::make_unique<QTimer>()}
{
    m_tableInsertTimer->setInterval(1000);
}

DemoSettingsPage::~DemoSettingsPage() = default;

QWidget* DemoSettingsPage::widget()
{
    if (m_widget == nullptr) { createWidget(); }
    return m_widget;
}

void DemoSettingsPage::createWidget()
{
    // NOLINTNEXTLINE
    m_widget = new QWidget();

    m_ui->setupUi(m_widget);

    connect(m_ui->pushButton, &QPushButton::clicked, m_tableInsertTimer.get(),
            qOverload<>(&QTimer::start));
    connect(m_tableInsertTimer.get(), &QTimer::timeout, this,
            &DemoSettingsPage::insertIntoTableWidget);
}

bool DemoSettingsPage::isModified() const
{
    if (m_widget == nullptr || m_ui == nullptr) { return false; }

    return m_ui->checkBox->isChecked() || m_ui->checkBox_2->isChecked() ||
           m_ui->radioButton_2->isChecked() ||
           m_ui->radioButton_4->isChecked() ||
           m_ui->comboBox->currentText() != "1" ||
           m_ui->fontComboBox->currentText() != "Ubuntu" ||
           m_ui->comboBox_2->currentText() != "1" ||
           m_ui->fontComboBox_2->currentText() != "Ubuntu" ||
           m_ui->spinBox->value() != 0 || m_ui->spinBox_2->value() != 0 ||
           m_ui->doubleSpinBox->value() != 0.0 ||
           m_ui->doubleSpinBox_2->value() != 0.0 ||
           m_ui->timeEdit->time() != QTime(0, 0, 0, 0) ||
           m_ui->timeEdit_2->time() != QTime(0, 0, 0, 0) ||
           m_ui->dateEdit->date() != QDate(2000, 1, 1) ||
           m_ui->dateEdit_2->date() != QDate(2000, 1, 1) ||
           m_ui->dateTimeEdit->dateTime() !=
               QDateTime(QDate(2000, 1, 1), QTime(0, 0, 0, 0)) ||
           m_ui->dateTimeEdit_2->dateTime() !=
               QDateTime(QDate(2000, 1, 1), QTime(0, 0, 0, 0)) ||
           m_ui->horizontalSlider->value() != 0 ||
           m_ui->horizontalSlider_2->value() != 0 ||
           m_ui->verticalSlider->value() != 0 ||
           m_ui->verticalSlider_2->value() != 0 || m_ui->dial->value() != 0 ||
           m_ui->dial_2->value() != 0 || m_ui->lineEdit->text() != "" ||
           m_ui->lineEdit_2->text() != "" ||
           m_ui->textEdit->toPlainText() != "" ||
           m_ui->textEdit_2->toPlainText() != "" ||
           m_ui->plainTextEdit->toPlainText() != "" ||
           m_ui->plainTextEdit_2->toPlainText() != "" ||
           m_ui->keySequenceEdit->keySequence() !=
               QKeySequence(Qt::CTRL + Qt::Key_F) ||
           m_ui->keySequenceEdit_2->keySequence() !=
               QKeySequence(Qt::CTRL + Qt::Key_F) ||
           m_ui->tableWidget->columnCount() > 0 ||
           m_ui->tableWidget->rowCount() > 0;
}

void DemoSettingsPage::insertIntoTableWidget()
{
    m_ui->tableWidget->setColumnCount(1);
    m_ui->tableWidget->setRowCount(1);
    // NOLINTNEXTLINE
    m_ui->tableWidget->setItem(0, 0, new QTableWidgetItem("Hello"));
}
