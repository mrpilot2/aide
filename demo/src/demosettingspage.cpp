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

    bool operator==(const WidgetState& rhs) const
    {
        return std::tie(
                   radioButtonChecked, radioButton2Checked, radioButton3Checked,
                   radioButton4Checked, checkBoxCheckState, checkBox2CheckState,
                   comboBoxIndex, comboBox2Index, fontComboBoxSelectedText,
                   fontComboBox2SelectedText, lineEditText, lineEdit2Text) ==
               std::tie(rhs.radioButtonChecked, rhs.radioButton2Checked,
                        rhs.radioButton3Checked, rhs.radioButton4Checked,
                        rhs.checkBoxCheckState, rhs.checkBox2CheckState,
                        rhs.comboBoxIndex, rhs.comboBox2Index,
                        rhs.fontComboBoxSelectedText,
                        rhs.fontComboBox2SelectedText, rhs.lineEditText,
                        rhs.lineEdit2Text);
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
        << obj.lineEditText << obj.lineEdit2Text;
    return out;
}

QDataStream& operator>>(QDataStream& in, WidgetState& obj)
{
    in >> obj.radioButtonChecked >> obj.radioButton2Checked >>
        obj.radioButton3Checked >> obj.radioButton4Checked >>
        obj.checkBoxCheckState >> obj.checkBox2CheckState >>
        obj.comboBoxIndex >> obj.comboBox2Index >>
        obj.fontComboBoxSelectedText >> obj.fontComboBox2SelectedText >>
        obj.lineEditText >> obj.lineEdit2Text;
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
}

void DemoSettingsPage::apply()
{
    m_settings->setValue(group(),
                         qVariantFromValue(widgetStateFromGui(m_ui.get())));
}
