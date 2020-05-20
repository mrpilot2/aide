
#include "mainwindow.hpp"

#include <QLibraryInfo>
#include <QTranslator>

#include "ui_mainwindow.h"

using aide::gui::MainWindow;

inline void initResources()
{
    Q_INIT_RESOURCE(translations);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
{
    initResources();

    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));

    QApplication::installTranslator(&qtTranslator);

    if (translator.load(QLocale(), QLatin1String("aide"), QLatin1String("_"),
                        QLatin1String(":/translations"))) {
        QApplication::installTranslator(&translator);
    }

    m_ui->setupUi(this);
}

MainWindow::~MainWindow() = default;