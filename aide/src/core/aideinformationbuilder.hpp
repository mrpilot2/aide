
#ifndef AIDE_AIDE_INFORMATION_BUILDER_HPP
#define AIDE_AIDE_INFORMATION_BUILDER_HPP

namespace aide::core
{
    struct AideInformation;

    // Builds the parts of AideInformation that are the same no matter which
    // feature is asking for it (version, git hash, compiler, build
    // type/flags, build date). Callers that need more than this common
    // subset (e.g. whatsNewUrl, thirdPartyLicensesHtml) fill those fields in
    // afterwards.
    class AideInformationBuilder
    {
    public:
        static AideInformation buildCurrent();
    };
} // namespace aide::core

#endif // AIDE_AIDE_INFORMATION_BUILDER_HPP
