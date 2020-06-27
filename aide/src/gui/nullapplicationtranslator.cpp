#include "applicationtranslator.hpp"

#include <QApplication>
#include <QLibraryInfo>
#include <QtCore/QDirIterator>
#include <QtGlobal>

#include <aide/logger.hpp>

using aide::gui::ApplicationTranslator;

ApplicationTranslator::ApplicationTranslator() = default;
