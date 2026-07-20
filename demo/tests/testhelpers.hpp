#ifndef DEMO_TESTS_TESTHELPERS_HPP
#define DEMO_TESTS_TESTHELPERS_HPP

#include <QPushButton>
#include <QString>
#include <QWidget>

namespace demo::test
{
    inline QPushButton* findButton(const QWidget& parent, const QString& text)
    {
        for (auto* button : parent.findChildren<QPushButton*>()) {
            if (button->text() == text) { return button; }
        }
        return nullptr;
    }
} // namespace demo::test

#endif // DEMO_TESTS_TESTHELPERS_HPP
