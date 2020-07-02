
#include "mainwindow.hpp"

#include <QObject>
#include <QPushButton>

#include "applicationtranslator.hpp"
#include "ui_mainwindow.h"

using aide::gui::MainWindow;
using aide::gui::TranslatorInterface;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_translator{new ApplicationTranslator}
    , m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
}
MainWindow::~MainWindow() = default;

std::shared_ptr<TranslatorInterface> MainWindow::translator() const
{
    return m_translator;
}
