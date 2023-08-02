
#ifndef AIDE_AIDE_INFORMATION_HPP
#define AIDE_AIDE_INFORMATION_HPP

#include <string>

#include <QDate>

#if defined(_MSC_VER)
#define ALIGNMENT(size) __declspec(align(size))
#elif defined(__GNUC__)
#define ALIGNMENT(size) __attribute__((aligned(size)))
#endif

namespace aide::core
{
    struct AideInformation
    {
        QDate buildDate;
        std::string copyrightYear;
        std::string gitHash;
        std::string versionInfo;
        std::string whatsNewUrl;

        std::string thirdPartyLicensesHtml;
    } ALIGNMENT(128);
} // namespace aide::core

#endif // AIDE_AIDE_INFORMATION_HPP
