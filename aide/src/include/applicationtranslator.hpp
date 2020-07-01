#ifndef AIDE_APPLICATIONTRANSLATOR_HPP
#define AIDE_APPLICATIONTRANSLATOR_HPP

#include <set>

#include <QDir>
#include <QTranslator>

#include <gui/translatorinterface.hpp>

namespace aide::gui
{
    class ApplicationTranslator : public TranslatorInterface
    {
    public:
        ApplicationTranslator();

        [[nodiscard]] std::set<std::string> getAvailableTranslations()
            const override;

    private:
        [[nodiscard]] std::set<std::string> fetchLanguages(
            const QStringList& languageFiles) const;

        QTranslator m_qtTranslator;
        QTranslator m_translator;

        std::vector<QDir> m_translationFilePaths;
    };
} // namespace aide::gui

#endif // AIDE_APPLICATIONTRANSLATOR_HPP
