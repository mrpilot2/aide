#include "changedetector.hpp"

#include <memory>

#include <QAbstractButton>
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QKeySequenceEdit>
#include <QLineEdit>
#include <QObject>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QTextEdit>

void installChangeDetectorForItemViews(
    const aide::gui::SettingsDialogController* settingsController,
    const QObject* child)
{
    if (const auto* itemView = qobject_cast<const QAbstractItemView*>(child);
        itemView != nullptr) {
        if (auto* itemModel = itemView->model(); itemModel != nullptr) {
            QObject::connect(
                itemModel, &QAbstractItemModel::columnsInserted,
                settingsController,
                &aide::gui::SettingsDialogController::onUserChangedAGuiElement);
            QObject::connect(
                itemModel, &QAbstractItemModel::rowsInserted,
                settingsController,
                &aide::gui::SettingsDialogController::onUserChangedAGuiElement);
            QObject::connect(
                itemModel, &QAbstractItemModel::columnsMoved,
                settingsController,
                &aide::gui::SettingsDialogController::onUserChangedAGuiElement);
            QObject::connect(
                itemModel, &QAbstractItemModel::rowsMoved, settingsController,
                &aide::gui::SettingsDialogController::onUserChangedAGuiElement);
            QObject::connect(
                itemModel, &QAbstractItemModel::columnsRemoved,
                settingsController,
                &aide::gui::SettingsDialogController::onUserChangedAGuiElement);
            QObject::connect(
                itemModel, &QAbstractItemModel::rowsRemoved, settingsController,
                &aide::gui::SettingsDialogController::onUserChangedAGuiElement);
        }
    }
}

void installChangeDetectorForTextEdits(
    const aide::gui::SettingsDialogController* settingsController,
    const QObject* child)
{
    if (const auto* lineEdit = qobject_cast<const QLineEdit*>(child);
        lineEdit != nullptr) {
        QObject::connect(
            lineEdit, &QLineEdit::textChanged, settingsController,
            &aide::gui::SettingsDialogController::onUserChangedAGuiElement);
    }
    if (const auto* textEdit = qobject_cast<const QTextEdit*>(child);
        textEdit != nullptr) {
        QObject::connect(
            textEdit, &QTextEdit::textChanged, settingsController,
            &aide::gui::SettingsDialogController::onUserChangedAGuiElement);
    }

    if (const auto* textEdit = qobject_cast<const QPlainTextEdit*>(child);
        textEdit != nullptr) {
        QObject::connect(
            textEdit, &QPlainTextEdit::textChanged, settingsController,
            &aide::gui::SettingsDialogController::onUserChangedAGuiElement);
    }
}
void installChangeDetectorForKeyEdit(
    const aide::gui::SettingsDialogController* settingsController,
    const QObject* child)
{
    if (const auto* keyEdit = qobject_cast<const QKeySequenceEdit*>(child);
        keyEdit != nullptr) {
        QObject::connect(
            keyEdit, &QKeySequenceEdit::keySequenceChanged, settingsController,
            &aide::gui::SettingsDialogController::onUserChangedAGuiElement);
    }
}
void installChangeDetectorForSliders(
    const aide::gui::SettingsDialogController* settingsController,
    const QObject* child)
{
    if (const auto* slider = qobject_cast<const QAbstractSlider*>(child);
        slider != nullptr) {
        QObject::connect(
            slider, &QAbstractSlider::valueChanged, settingsController,
            &aide::gui::SettingsDialogController::onUserChangedAGuiElement);
    }
}
void installChangeDetectorsForDateTimeEdits(
    const aide::gui::SettingsDialogController* settingsController,
    const QObject* child)
{
    if (const auto* dateTimeEdit = qobject_cast<const QDateTimeEdit*>(child);
        dateTimeEdit != nullptr) {
        QObject::connect(
            dateTimeEdit, &QDateTimeEdit::dateChanged, settingsController,
            &aide::gui::SettingsDialogController::onUserChangedAGuiElement);
        QObject::connect(
            dateTimeEdit, &QDateTimeEdit::timeChanged, settingsController,
            &aide::gui::SettingsDialogController::onUserChangedAGuiElement);
        QObject::connect(
            dateTimeEdit, &QDateTimeEdit::dateTimeChanged, settingsController,
            &aide::gui::SettingsDialogController::onUserChangedAGuiElement);
    }
}
void installChangeDetectorForSpinBoxes(
    const aide::gui::SettingsDialogController* settingsController,
    const QObject* child)
{
    if (const auto* spinBox = qobject_cast<const QSpinBox*>(child);
        spinBox != nullptr) {
        QObject::connect(
            spinBox, qOverload<int>(&QSpinBox::valueChanged),
            settingsController,
            &aide::gui::SettingsDialogController::onUserChangedAGuiElement);
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        QObject::connect(
            spinBox, &QSpinBox::textChanged, settingsController,
            &aide::gui::SettingsDialogController::onUserChangedAGuiElement);
#else
        QObject::connect(
            spinBox, qOverload<const QString&>(&QSpinBox::valueChanged),
            settingsController,
            &aide::gui::SettingsDialogController::onUserChangedAGuiElement);
#endif
    }
    if (const auto* spinBox = qobject_cast<const QDoubleSpinBox*>(child);
        spinBox != nullptr) {
        QObject::connect(
            spinBox, qOverload<double>(&QDoubleSpinBox::valueChanged),
            settingsController,
            &aide::gui::SettingsDialogController::onUserChangedAGuiElement);
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        QObject::connect(
            spinBox, &QDoubleSpinBox::textChanged, settingsController,
            &aide::gui::SettingsDialogController::onUserChangedAGuiElement);
#else
        QObject::connect(
            spinBox, qOverload<const QString&>(&QDoubleSpinBox::valueChanged),
            settingsController,
            &aide::gui::SettingsDialogController::onUserChangedAGuiElement);
#endif
    }
}
void installChangeDetectorForComboBoxes(
    const aide::gui::SettingsDialogController* settingsController,
    const QObject* child)
{
    if (const auto* comboBox = qobject_cast<const QComboBox*>(child);
        comboBox != nullptr) {
        QObject::connect(
            comboBox, &QComboBox::editTextChanged, settingsController,
            &aide::gui::SettingsDialogController::onUserChangedAGuiElement);
        QObject::connect(
            comboBox, qOverload<int>(&QComboBox::currentIndexChanged),
            settingsController,
            &aide::gui::SettingsDialogController::onUserChangedAGuiElement);
        QObject::connect(
            comboBox, &QComboBox::currentTextChanged, settingsController,
            &aide::gui::SettingsDialogController::onUserChangedAGuiElement);
    }
}
void installChangeDetectorForButtons(
    const aide::gui::SettingsDialogController* settingsController,
    const QObject* child)
{
    if (const auto* button = qobject_cast<const QAbstractButton*>(child);
        button != nullptr) {
        QObject::connect(
            button, &QAbstractButton::clicked, settingsController,
            &aide::gui::SettingsDialogController::onUserChangedAGuiElement);
    }
}

void aide::gui::installChangeDetector(
    QObject* widget, const SettingsDialogControllerPtr& controller)
{
    auto* settingsController = controller.get();

    for (auto* child : widget->children()) {
        installChangeDetector(child, controller);

        installChangeDetectorForButtons(settingsController, child);

        installChangeDetectorForComboBoxes(settingsController, child);

        installChangeDetectorForSpinBoxes(settingsController, child);

        installChangeDetectorsForDateTimeEdits(settingsController, child);

        installChangeDetectorForSliders(settingsController, child);

        installChangeDetectorForKeyEdit(settingsController, child);

        installChangeDetectorForTextEdits(settingsController, child);

        installChangeDetectorForItemViews(settingsController, child);
    }
}
