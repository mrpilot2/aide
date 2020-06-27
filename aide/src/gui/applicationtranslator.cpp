#include "applicationtranslator.hpp"

#include <QApplication>
#include <QLibraryInfo>
#include <QtGlobal>

using aide::gui::ApplicationTranslator;

inline void initResources()
{
    Q_INIT_RESOURCE(translations);
}

ApplicationTranslator::ApplicationTranslator()
{
    initResources();

    m_qtTranslator.load("qt_" + QLocale::system().name(),
                        QLibraryInfo::location(QLibraryInfo::TranslationsPath));

    QApplication::installTranslator(&m_qtTranslator);

    if (m_translator.load(QLocale(), QLatin1String("aide"), QLatin1String("_"),
                          QLatin1String(":/translations"))) {
        QApplication::installTranslator(&m_translator);
    }
}
