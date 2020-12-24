/*
 * NOTE: this file is required for CI systems that do not have QLinguistTools
 * installed. This is for example the case on lgtm.com
 */

#include "applicationtranslator.hpp"

using aide::gui::ApplicationTranslator;

ApplicationTranslator::ApplicationTranslator() = default;

std::set<std::string> ApplicationTranslator::getAvailableTranslations() const
{
    return std::set<std::string>();
}

void ApplicationTranslator::addAdditionalTranslationFilePath(
    [[maybe_unused]] const QDir& path, [[maybe_unused]] const QString& fileName)
{}
