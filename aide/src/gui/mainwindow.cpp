
#include "mainwindow.hpp"

#include "ui_mainwindow.h"

using aide::gui::MainWindow;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
}

MainWindow::~MainWindow() = default;