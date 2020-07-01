#include "applicationtranslator.hpp"

#include <QApplication>
#include <QLibraryInfo>
#include <QtCore/QDirIterator>
#include <QtGlobal>

#include <aide/logger.hpp>

using aide::gui::ApplicationTranslator;

const QDir libraryTranslationPath{":/aide_library_translations"};

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
                          libraryTranslationPath.absolutePath())) {
        QApplication::installTranslator(&m_translator);
    }
}
std::set<std::string> ApplicationTranslator::getAvailableTranslations() const
{
    std::set<std::string> languages{};

    auto languageFiles = libraryTranslationPath.entryList(QStringList("*.qm"));

    const auto languagesInCurrentPath{fetchLanguages(languageFiles)};
    languages.insert(languagesInCurrentPath.begin(),
                     languagesInCurrentPath.end());
    return languages;
}

std::set<std::string> ApplicationTranslator::fetchLanguages(
    const QStringList& languageFiles) const
{
    std::set<std::string> languages;

    for (const auto& languageFile : languageFiles) {
        const int start            = languageFile.indexOf(QLatin1Char('_')) + 1;
        const int end              = languageFile.lastIndexOf(QLatin1Char('.'));
        const QString localeString = languageFile.mid(start, end - start);

        const QLocale locale{localeString};

        const QString languageString(
            QLocale::languageToString(locale.language()) + QLatin1String(" (") +
            QLocale::countryToString(locale.country()) + QLatin1Char(')'));
        languages.emplace(languageString.toStdString());
        AIDE_LOG_INFO("Found language {} in file {}",
                      languageFile.toStdString(), languageFile.toStdString())
    }
    return languages;
}
