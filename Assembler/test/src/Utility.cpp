#include "doctest/doctest.h"

#include "Utility.hpp"

TEST_CASE("Testing Trim()")
{
    CHECK(""     == Trim(""));
    CHECK(""     == Trim("    "));
    CHECK("Test" == Trim("Test"));

    SUBCASE("Left trim test")
    {
        SUBCASE("Without space in word")
        {
            CHECK("Test" == Trim(" Test"));
            CHECK("Test" == Trim("   Test"));
            CHECK("Test" == Trim("           Test"));
        }

        SUBCASE("With space in word")
        {
            CHECK("Te   st"     == Trim(" Te   st"));
            CHECK("Te    st"    == Trim("   Te    st"));
            CHECK("Te       st" == Trim("           Te       st"));
        }
    }

    SUBCASE("Right trim test")
    {
        SUBCASE("Without space in word")
        {
            CHECK("Test" == Trim("Test "));
            CHECK("Test" == Trim("Test   "));
            CHECK("Test" == Trim("Test           "));
        }

        SUBCASE("With space in word")
        {
            CHECK("Te   st"     == Trim("Te   st "));
            CHECK("Te    st"    == Trim("Te    st   "));
            CHECK("Te       st" == Trim("Te       st           "));
        }
    }

    SUBCASE("Right and left trim test")
    {
        SUBCASE("Without space in word")
        {
            CHECK("Test" == Trim(" Test "));
            CHECK("Test" == Trim("   Test   "));
            CHECK("Test" == Trim("           Test           "));

            CHECK("Test" == Trim("   Test "));
            CHECK("Test" == Trim("      Test   "));
            CHECK("Test" == Trim("             Test         "));
        }

        SUBCASE("With space in word")
        {
            CHECK("Te   st"     == Trim(" Te   st "));
            CHECK("Te    st"    == Trim("   Te    st   "));
            CHECK("Te       st" == Trim("           Te       st           "));

            CHECK("Te       st" == Trim("   Te       st "));
            CHECK("Te       st" == Trim("      Te       st   "));
            CHECK("Te       st" == Trim("             Te       st         "));
        }
    }
}