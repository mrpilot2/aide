#include "applicationtranslator.hpp"

#include <QApplication>
#include <QLibraryInfo>
#include <QtCore/QDirIterator>
#include <QtGlobal>

#include <aide/logger.hpp>

using aide::gui::ApplicationTranslator;

ApplicationTranslator::ApplicationTranslator() = default;

std::vector<std::string>
aide::gui::ApplicationTranslator::getAvailableTranslations() const
{
    return std::vector<std::string>();
}
