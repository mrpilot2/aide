#include "applicationtranslator.hpp"

#include <QApplication>
#include <QLibraryInfo>
#include <QtCore/QDirIterator>
#include <QtGlobal>

#include <aide/logger.hpp>

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
                          QLatin1String(":/aide_library_translations"))) {
        QApplication::installTranslator(&m_translator);
    }
}

std::vector<std::string>
aide::gui::ApplicationTranslator::getAvailableTranslations() const
{
    std::vector<std::string> languages{"English", "Deutsch"};

    return languages;
}
