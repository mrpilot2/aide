
#ifndef AIDE_DIAGNOSTIC_TEXT_BUILDER_HPP
#define AIDE_DIAGNOSTIC_TEXT_BUILDER_HPP

#include <QString>

namespace aide::core
{
    struct AideInformation;

    class DiagnosticTextBuilder
    {
    public:
        static QString build(const AideInformation& info);
    };
} // namespace aide::core

#endif // AIDE_DIAGNOSTIC_TEXT_BUILDER_HPP
