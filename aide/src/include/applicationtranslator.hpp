#ifndef AIDE_APPLICATIONTRANSLATOR_HPP
#define AIDE_APPLICATIONTRANSLATOR_HPP

#include <memory>
#include <set>

#include <QDir>
#include <QMap>
#include <QTranslator>

#include <gui/translatorinterface.hpp>

namespace aide::gui
{
    class ApplicationTranslator : public TranslatorInterface
    {
    public:
        ApplicationTranslator();

        void addAdditionalTranslationFilePath(const QDir& path,
                                              const QString& fileName) override;

        [[nodiscard]] std::set<std::string> getAvailableTranslations()
            const override;

    private:
        [[nodiscard]] static std::set<std::string> fetchLanguages(
            const QStringList& languageFiles);

        void installNewTranslator(const QDir& path, const QString& fileName);

        static QString extractLocaleFromFileName(const QString& languageFile);

        QTranslator m_qtTranslator;
        std::vector<std::shared_ptr<QTranslator>> m_translator;
        std::map<std::string, std::string> m_translationFilePaths;
    };
} // namespace aide::gui

#endif // AIDE_APPLICATIONTRANSLATOR_HPP
