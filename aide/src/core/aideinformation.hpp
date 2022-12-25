
#ifndef AIDE_AIDE_INFORMATION_HPP
#define AIDE_AIDE_INFORMATION_HPP

#include <string>

#include <QDate>

namespace aide::core
{
    struct AideInformation
    {
        QDate buildDate;
        std::string copyrightYear;
        std::string gitHash;
        std::string versionInfo;
    };
} // namespace aide::core

#endif // AIDE_AIDE_INFORMATION_HPP
