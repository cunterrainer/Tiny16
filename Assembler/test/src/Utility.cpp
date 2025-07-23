#include "doctest/doctest.h"

#include "Utility.hpp"

TEST_CASE("Testing Trim()")
{
    CHECK(Trim("   a   ") == "a");
}