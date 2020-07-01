/*
 * NOTE: this file is required for CI systems that do not have QLinguistTools
 * installed. This is for example the case on lgtm.com
 */

#include <QApplication>
#include <QLibraryInfo>
#include <QtCore/QDirIterator>
#include <QtGlobal>

#include <aide/logger.hpp>

#include "applicationtranslator.hpp"

using aide::gui::ApplicationTranslator;

ApplicationTranslator::ApplicationTranslator() = default;

std::set<std::string> ApplicationTranslator::getAvailableTranslations() const
{
    return std::set<std::string>();
}

void ApplicationTranslator::addAdditionalTranslationFilePath(
    const QDir &path, const QString &fileName)
{}
