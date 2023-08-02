

#include "loggerfactory.hpp"

#include <iostream>

#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QString>

#include "logger.hpp"

using aide::LoggerPtr;
using aide::core::LoggerFactory;

LoggerPtr aide::core::LoggerFactory::createLogger()
{
    return setupLogger("aide");
}

LoggerPtr LoggerFactory::createLogger(const std::string& loggerName)
{
    return setupLogger(loggerName);
}

LoggerPtr LoggerFactory::setupLogger(const std::string& loggerName)
{
    QString logLocation(
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation));

    bool success = tryToCreateLogLocationIfItDoesNotExist(logLocation);

    if (!success) {
        logLocation =
            QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        success = tryToCreateLogLocationIfItDoesNotExist(logLocation);
    }

    if (success) {
        FileName logPath(logLocation.append("/")
                             .append(QApplication::applicationName())
                             .append(".log")
                             .toStdString());

        auto logger =
            std::make_shared<aide::Logger>(logPath, LoggerName(loggerName));

        if (loggerName == "aide") {
            logger->info("Configured logger to log to file {}", logPath());
            logger->flush();
        }
        return logger;
    }
    return std::make_shared<Logger>();
}

bool LoggerFactory::tryToCreateLogLocationIfItDoesNotExist(
    const QString& logLocation)
{
    if (!logLocation.isEmpty()) {
        QDir d;
        if (!d.mkpath(logLocation)) {
            std::cerr << "Could not create standard log directory: "
                      << logLocation.toStdString()
                      << ". Trying to log into temp directory instead. If this "
                         "also does not work no file logging will happen.";
            return false;
        }
        return true;
    }
    return false;
}
