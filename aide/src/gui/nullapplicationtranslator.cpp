#include <QApplication>
#include <QLibraryInfo>
#include <QtCore/QDirIterator>
#include <QtGlobal>

#include <aide/logger.hpp>

#include "applicationtranslator.hpp"

using aide::gui::ApplicationTranslator;

ApplicationTranslator::ApplicationTranslator() = default;

std::set<std::string>
aide::gui::ApplicationTranslator::getAvailableTranslations() const
{
    return std::vector<std::string>();
}
