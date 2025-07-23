#include "doctest/doctest.h"

#include "Parser.hpp"
#include "Validator.hpp"

TEST_CASE("Test ValidateInstruction()")
{
    SUBCASE("Invalid instruction")
    {
        ValidationResult result = ValidateInstruction(ParseLine("HLT R1", 0).value());
        CHECK(result.valid == false);

        result = ValidateInstruction(ParseLine("HLT R1, R2", 0).value());
        CHECK(result.valid == false);

        result = ValidateInstruction(ParseLine("JMP", 0).value());
        CHECK(result.valid == false);

        result = ValidateInstruction(ParseLine("JE", 0).value());
        CHECK(result.valid == false);

        result = ValidateInstruction(ParseLine("JMP R1, R2", 0).value());
        CHECK(result.valid == false);

        result = ValidateInstruction(ParseLine("JE R1, R2", 0).value());
        CHECK(result.valid == false);
    }


    SUBCASE("Valid Instruction")
    {
        ValidationResult result = ValidateInstruction(ParseLine("HLT", 0).value());
        CHECK(result.valid == true);
    }
}