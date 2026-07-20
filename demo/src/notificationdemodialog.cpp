#include "notificationdemodialog.hpp"

#include <array>

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>

#include <aide/notificationtype.hpp>

using aide::NotificationType;
using aide::widgets::AideDialog;
using demo::NotificationDemoDialog;

namespace
{
    struct SeverityButton
    {
        NotificationType type;
        QString buttonLabel;
        QString message;
    };
} // namespace

NotificationDemoDialog::NotificationDemoDialog(QWidget* parent)
    : AideDialog(parent)
{
    setWindowTitle(tr("Dialog Banner Demo"));

    auto* buttonsRow = new QHBoxLayout;

    const std::array<SeverityButton, 4> severityButtons{{
        {NotificationType::Information, tr("Info"),
         tr("This is an informational banner.")},
        {NotificationType::Success, tr("Success"),
         tr("This is a success banner.")},
        {NotificationType::Warning, tr("Warning"),
         tr("This is a warning banner.")},
        {NotificationType::Error, tr("Error"), tr("This is an error banner.")},
    }};

    for (const auto& severityButton : severityButtons) {
        auto* button = new QPushButton(severityButton.buttonLabel, this);
        connect(button, &QPushButton::clicked, this, [this, severityButton]() {
            showBanner(severityButton.type, severityButton.message);
        });
        buttonsRow->addWidget(button);
    }

    auto* clearButton = new QPushButton(tr("Clear"), this);
    connect(clearButton, &QPushButton::clicked, this, &AideDialog::clearBanner);
    buttonsRow->addWidget(clearButton);

    contentLayout()->addLayout(buttonsRow);

    auto* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    contentLayout()->addWidget(buttonBox);
}
