#include "demosettingspage.hpp"

#include <utility>

#include <QPushButton>
#include <QWidget>

#include <aide/hierarchicalid.hpp>
#include <aide/settingsinterface.hpp>

#include "ui_demosettingspage.h"

using aide::HierarchicalId;
using aide::core::SettingsPage;

struct WidgetState
{
    bool radioButtonChecked{true};
    bool radioButton2Checked{false};
    bool radioButton3Checked{false};
    bool radioButton4Checked{true};
    int checkBoxCheckState{0};
    int checkBox2CheckState{1};
    int comboBoxIndex{1};
    int comboBox2Index{0};
    QString fontComboBoxSelectedText{"Ubuntu"};
    QString fontComboBox2SelectedText{"Arial"};
    QString lineEditText{"Demo text"};
    QString lineEdit2Text{"Another demo text"};
    int spinBoxValue{5};
    int spinBox2Value{10};
    double doubleSpinBoxValue{17.3};
    double doubleSpinBox2Value{23.3};
    int verticalSliderValue{10};
    int verticalSlider2Value{37};
    int horizontalSliderValue{25};
    int horizontalSlider2Value{5};
    QKeySequence keySequenceEditValue{"Ctrl + F"};
    QKeySequence keySequenceEdit2Value{"Ctrl + Alt + F4"};
    int dialValue{55};
    int dial2Value{24};
    QTime timeEditValue{13, 30, 24};
    QTime timeEdit2Value{22, 17, 39};
    QDate dateEditValue{2020, 3, 27};
    QDate dateEdit2Value{2017, 5, 13};
    QDateTime dateTimeValue{QDate{2013, 1, 22}, QTime{8, 1, 3}};
    QDateTime dateTime2Value{QDate{2018, 4, 18}, QTime{10, 11, 12}};
    QString textEditValue{"Demo text edit"};
    QString textEdit2Value{"Demo text edit 2"};
    QString plainTextEditValue{"Demo plain text edit"};
    QString plainTextEdit2Value{"Demo plain text edit 2"};

    bool operator==(const WidgetState& rhs) const
    {
        return std::tie(
                   radioButtonChecked, radioButton2Checked, radioButton3Checked,
                   radioButton4Checked, checkBoxCheckState, checkBox2CheckState,
                   comboBoxIndex, comboBox2Index, fontComboBoxSelectedText,
                   fontComboBox2SelectedText, lineEditText, lineEdit2Text,
                   spinBoxValue, spinBox2Value, doubleSpinBoxValue,
                   doubleSpinBox2Value, verticalSliderValue,
                   verticalSlider2Value, horizontalSliderValue,
                   horizontalSlider2Value, keySequenceEditValue,
                   keySequenceEdit2Value, dialValue, dial2Value, timeEditValue,
                   timeEdit2Value, dateEditValue, dateEdit2Value, dateTimeValue,
                   dateTime2Value, textEditValue, textEdit2Value,
                   plainTextEditValue, plainTextEdit2Value) ==
               std::tie(
                   rhs.radioButtonChecked, rhs.radioButton2Checked,
                   rhs.radioButton3Checked, rhs.radioButton4Checked,
                   rhs.checkBoxCheckState, rhs.checkBox2CheckState,
                   rhs.comboBoxIndex, rhs.comboBox2Index,
                   rhs.fontComboBoxSelectedText, rhs.fontComboBox2SelectedText,
                   rhs.lineEditText, rhs.lineEdit2Text, rhs.spinBoxValue,
                   rhs.spinBox2Value, rhs.doubleSpinBoxValue,
                   rhs.doubleSpinBox2Value, rhs.verticalSliderValue,
                   rhs.verticalSlider2Value, rhs.horizontalSliderValue,
                   rhs.horizontalSlider2Value, rhs.keySequenceEditValue,
                   rhs.keySequenceEdit2Value, rhs.dialValue, rhs.dial2Value,
                   rhs.timeEditValue, rhs.timeEdit2Value, rhs.dateEditValue,
                   rhs.dateEdit2Value, rhs.dateTimeValue, rhs.dateTime2Value,
                   rhs.textEditValue, rhs.textEdit2Value,
                   rhs.plainTextEditValue, rhs.plainTextEdit2Value);
    }
    bool operator!=(const WidgetState& rhs) const { return !(rhs == *this); }
};
// NOLINTNEXTLINE
Q_DECLARE_METATYPE(WidgetState)

QDataStream& operator<<(QDataStream& out, const WidgetState& obj)
{
    out << obj.radioButtonChecked << obj.radioButton2Checked
        << obj.radioButton3Checked << obj.radioButton4Checked
        << obj.checkBoxCheckState << obj.checkBox2CheckState
        << obj.comboBoxIndex << obj.comboBox2Index
        << obj.fontComboBoxSelectedText << obj.fontComboBox2SelectedText
        << obj.lineEditText << obj.lineEdit2Text << obj.spinBoxValue
        << obj.spinBox2Value << obj.doubleSpinBoxValue
        << obj.doubleSpinBox2Value << obj.verticalSliderValue
        << obj.verticalSlider2Value << obj.horizontalSliderValue
        << obj.horizontalSlider2Value << obj.keySequenceEditValue
        << obj.keySequenceEdit2Value << obj.dialValue << obj.dial2Value
        << obj.timeEditValue << obj.timeEdit2Value << obj.dateEditValue
        << obj.dateEdit2Value << obj.dateTimeValue << obj.dateTime2Value
        << obj.textEditValue << obj.textEdit2Value << obj.plainTextEditValue
        << obj.plainTextEdit2Value;
    return out;
}

QDataStream& operator>>(QDataStream& in, WidgetState& obj)
{
    in >> obj.radioButtonChecked >> obj.radioButton2Checked >>
        obj.radioButton3Checked >> obj.radioButton4Checked >>
        obj.checkBoxCheckState >> obj.checkBox2CheckState >>
        obj.comboBoxIndex >> obj.comboBox2Index >>
        obj.fontComboBoxSelectedText >> obj.fontComboBox2SelectedText >>
        obj.lineEditText >> obj.lineEdit2Text >> obj.spinBoxValue >>
        obj.spinBox2Value >> obj.doubleSpinBoxValue >>
        obj.doubleSpinBox2Value >> obj.verticalSliderValue >>
        obj.verticalSlider2Value >> obj.horizontalSliderValue >>
        obj.horizontalSlider2Value >> obj.keySequenceEditValue >>
        obj.keySequenceEdit2Value >> obj.dialValue >> obj.dial2Value >>
        obj.timeEditValue >> obj.timeEdit2Value >> obj.dateEditValue >>
        obj.dateEdit2Value >> obj.dateTimeValue >> obj.dateTime2Value >>
        obj.textEditValue >> obj.textEdit2Value >> obj.plainTextEditValue >>
        obj.plainTextEdit2Value;
    return in;
}

WidgetState widgetStateFromGui(Ui::DemoSettingsPage* ui)
{
    WidgetState widgetState;

    widgetState.radioButtonChecked        = ui->radioButton->isChecked();
    widgetState.radioButton2Checked       = ui->radioButton_2->isChecked();
    widgetState.radioButton3Checked       = ui->radioButton_3->isChecked();
    widgetState.radioButton4Checked       = ui->radioButton_4->isChecked();
    widgetState.checkBoxCheckState        = ui->checkBox->checkState();
    widgetState.checkBox2CheckState       = ui->checkBox_2->checkState();
    widgetState.comboBoxIndex             = ui->comboBox->currentIndex();
    widgetState.comboBox2Index            = ui->comboBox_2->currentIndex();
    widgetState.fontComboBoxSelectedText  = ui->fontComboBox->currentText();
    widgetState.fontComboBox2SelectedText = ui->fontComboBox_2->currentText();
    widgetState.lineEditText              = ui->lineEdit->text().trimmed();
    widgetState.lineEdit2Text             = ui->lineEdit_2->text().trimmed();
    widgetState.spinBoxValue              = ui->spinBox->value();
    widgetState.spinBox2Value             = ui->spinBox_2->value();
    widgetState.doubleSpinBoxValue        = ui->doubleSpinBox->value();
    widgetState.doubleSpinBox2Value       = ui->doubleSpinBox_2->value();
    widgetState.verticalSliderValue       = ui->verticalSlider->value();
    widgetState.verticalSlider2Value      = ui->verticalSlider_2->value();
    widgetState.horizontalSliderValue     = ui->horizontalSlider->value();
    widgetState.horizontalSlider2Value    = ui->horizontalSlider_2->value();
    widgetState.keySequenceEditValue      = ui->keySequenceEdit->keySequence();
    widgetState.keySequenceEdit2Value = ui->keySequenceEdit_2->keySequence();
    widgetState.dialValue             = ui->dial->value();
    widgetState.dial2Value            = ui->dial_2->value();
    widgetState.timeEditValue         = ui->timeEdit->time();
    widgetState.timeEdit2Value        = ui->timeEdit_2->time();
    widgetState.dateEditValue         = ui->dateEdit->date();
    widgetState.dateEdit2Value        = ui->dateEdit_2->date();
    widgetState.dateTimeValue         = ui->dateTimeEdit->dateTime();
    widgetState.dateTime2Value        = ui->dateTimeEdit_2->dateTime();
    widgetState.textEditValue         = ui->textEdit->toPlainText().trimmed();
    widgetState.textEdit2Value        = ui->textEdit_2->toPlainText().trimmed();
    widgetState.plainTextEditValue = ui->plainTextEdit->toPlainText().trimmed();
    widgetState.plainTextEdit2Value =
        ui->plainTextEdit_2->toPlainText().trimmed();
    return widgetState;
}

DemoSettingsPage::DemoSettingsPage(
    aide::HierarchicalId group,
    std::shared_ptr<aide::SettingsInterface> settings)
    : SettingsPage(std::move(group))
    , m_ui{std::make_unique<Ui::DemoSettingsPage>()}
    , m_settings{std::move(settings)}
    , m_widget(nullptr)
    , m_tableInsertTimer{std::make_unique<QTimer>()}
{
    qRegisterMetaType<WidgetState>("WidgetState");
    qRegisterMetaTypeStreamOperators<WidgetState>("WidgetState");

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

    WidgetState defaultWidgetState;

    auto widgetStateFromSettings =
        m_settings->value(group(), qVariantFromValue(defaultWidgetState))
            .value<WidgetState>();

    return widgetStateFromSettings != widgetStateFromGui(m_ui.get());
}

void DemoSettingsPage::insertIntoTableWidget()
{
    m_ui->tableWidget->setColumnCount(1);
    m_ui->tableWidget->setRowCount(1);
    // NOLINTNEXTLINE
    m_ui->tableWidget->setItem(0, 0, new QTableWidgetItem("Hello"));
}

void DemoSettingsPage::reset()
{
    if (m_widget == nullptr || m_ui == nullptr) { createWidget(); }

    WidgetState defaultWidgetState;
    auto widgetState =
        m_settings->value(group(), qVariantFromValue(defaultWidgetState))
            .value<WidgetState>();

    m_ui->radioButton->setChecked(widgetState.radioButtonChecked);
    m_ui->radioButton_2->setChecked(widgetState.radioButton2Checked);
    m_ui->radioButton_3->setChecked(widgetState.radioButton3Checked);
    m_ui->radioButton_4->setChecked(widgetState.radioButton4Checked);
    m_ui->checkBox->setCheckState(
        static_cast<Qt::CheckState>(widgetState.checkBoxCheckState));
    m_ui->checkBox_2->setCheckState(
        static_cast<Qt::CheckState>(widgetState.checkBox2CheckState));

    m_ui->comboBox->setCurrentIndex(widgetState.comboBoxIndex);
    m_ui->comboBox_2->setCurrentIndex(widgetState.comboBox2Index);
    m_ui->fontComboBox->setCurrentText(widgetState.fontComboBoxSelectedText);
    m_ui->fontComboBox_2->setCurrentText(widgetState.fontComboBox2SelectedText);

    m_ui->lineEdit->setText(widgetState.lineEditText.trimmed());
    m_ui->lineEdit_2->setText(widgetState.lineEdit2Text.trimmed());

    m_ui->spinBox->setValue(widgetState.spinBoxValue);
    m_ui->spinBox_2->setValue(widgetState.spinBox2Value);
    m_ui->doubleSpinBox->setValue(widgetState.doubleSpinBoxValue);
    m_ui->doubleSpinBox_2->setValue(widgetState.doubleSpinBox2Value);

    m_ui->verticalSlider->setValue(widgetState.verticalSliderValue);
    m_ui->verticalSlider_2->setValue(widgetState.verticalSlider2Value);
    m_ui->horizontalSlider->setValue(widgetState.horizontalSliderValue);
    m_ui->horizontalSlider_2->setValue(widgetState.horizontalSlider2Value);

    m_ui->keySequenceEdit->setKeySequence(widgetState.keySequenceEditValue);
    m_ui->keySequenceEdit_2->setKeySequence(widgetState.keySequenceEdit2Value);

    m_ui->dial->setValue(widgetState.dialValue);
    m_ui->dial_2->setValue(widgetState.dial2Value);

    m_ui->timeEdit->setTime(widgetState.timeEditValue);
    m_ui->timeEdit_2->setTime(widgetState.timeEdit2Value);
    m_ui->dateEdit->setDate(widgetState.dateEditValue);
    m_ui->dateEdit_2->setDate(widgetState.dateEdit2Value);
    m_ui->dateTimeEdit->setDateTime(widgetState.dateTimeValue);
    m_ui->dateTimeEdit_2->setDateTime(widgetState.dateTime2Value);

    m_ui->textEdit->setText(widgetState.textEditValue.trimmed());
    m_ui->textEdit_2->setText(widgetState.textEdit2Value.trimmed());

    m_ui->plainTextEdit->setPlainText(widgetState.plainTextEditValue.trimmed());
    m_ui->plainTextEdit_2->setPlainText(
        widgetState.plainTextEdit2Value.trimmed());
}

void DemoSettingsPage::apply()
{
    m_settings->setValue(group(),
                         qVariantFromValue(widgetStateFromGui(m_ui.get())));
}
