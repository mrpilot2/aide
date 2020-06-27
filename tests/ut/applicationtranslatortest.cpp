#include "catch2/catch.hpp"

#include "applicationtranslator.hpp"

using aide::gui::ApplicationTranslator;

TEST_CASE("ApplicationTranslator finds at least two languages")
{
    ApplicationTranslator translator;

    REQUIRE(translator.getAvailableTranslations().size() >= 2);
}
