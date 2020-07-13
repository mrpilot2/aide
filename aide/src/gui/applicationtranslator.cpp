#include "applicationtranslator.hpp"

#include <QApplication>
#include <QLibraryInfo>
#include <QtCore/QDirIterator>
#include <QtGlobal>

#include <aide/logger.hpp>

using aide::gui::ApplicationTranslator;

const QDir& libraryTranslationPath()
{
    try {
        static const QDir libraryTranslationPath{":/aide_library_translations"};
        return libraryTranslationPath;
    }
    catch (...) {
        AIDE_LOG_ERROR(
            "ApplicationTranslator: Could not create static storage duration "
            "library translation path")
        std::terminate();
    }
}

inline void initResources()
{
    Q_INIT_RESOURCE(translations);
}

ApplicationTranslator::ApplicationTranslator()
{
    initResources();

    AIDE_LOG_INFO(
        "Current application language is {}",
        QLocale::languageToString(QLocale::system().language()).toStdString())

    m_qtTranslator.load("qt_" + QLocale::system().name(),
                        QLibraryInfo::location(QLibraryInfo::TranslationsPath));

    QApplication::installTranslator(&m_qtTranslator);

    installNewTranslator(libraryTranslationPath(), QLatin1String("aide"));
}

void ApplicationTranslator::addAdditionalTranslationFilePath(
    const QDir& path, const QString& fileName)
{
    m_translationFilePaths.insert(std::make_pair(
        path.absolutePath().toStdString(), fileName.toStdString()));

    installNewTranslator(path, fileName);
}

void ApplicationTranslator::installNewTranslator(const QDir& path,
                                                 const QString& fileName)
{
    auto currentTranslator =
        m_translator.emplace_back(std::make_shared<QTranslator>());
    if (currentTranslator->load(QLocale(), fileName, QLatin1String("_"),
                                path.absolutePath())) {
        QApplication::installTranslator(currentTranslator.get());
    }
}

std::set<std::string> ApplicationTranslator::getAvailableTranslations() const
{
    std::set<std::string> appLanguages{};

    for (const auto& pair : m_translationFilePaths) {
        const QDir directory(QString::fromStdString(pair.first));
        auto appLanguageFiles = directory.entryList(
            QStringList(QString::fromStdString(pair.second) + "*.qm"));

        const auto appLanguagesInCurrentPath{fetchLanguages(appLanguageFiles)};
        appLanguages.insert(appLanguagesInCurrentPath.begin(),
                            appLanguagesInCurrentPath.end());
    }

    if (appLanguages.empty()) {
        std::set<std::string> languages{};

        auto languageFiles =
            libraryTranslationPath().entryList(QStringList("*.qm"));

        const auto languagesInCurrentPath{fetchLanguages(languageFiles)};
        languages.insert(languagesInCurrentPath.begin(),
                         languagesInCurrentPath.end());
        return languages;
    }
    return appLanguages;
}

std::set<std::string> ApplicationTranslator::fetchLanguages(
    const QStringList& languageFiles)
{
    std::set<std::string> languages;

    for (const auto& languageFile : languageFiles) {
        const QLocale locale{extractLocaleFromFileName(languageFile)};

        const QString languageString(
            QLocale::languageToString(locale.language()) + QLatin1String(" (") +
            QLocale::countryToString(locale.country()) + QLatin1Char(')'));

        languages.emplace(languageString.toStdString());
        AIDE_LOG_DEBUG("Found language {} in file {}",
                       languageString.toStdString(), languageFile.toStdString())
    }
    return languages;
}

QString ApplicationTranslator::extractLocaleFromFileName(
    const QString& languageFile)
{
    const int start = languageFile.indexOf(QLatin1Char('_')) + 1;
    const int end   = languageFile.lastIndexOf(QLatin1Char('.'));
    return languageFile.mid(start, end - start);
}
