#include "doctest/doctest.h"

#include "Parser.hpp"

TEST_CASE("Testing Tokenize()")
{
    auto tokens = Tokenize("MOV $1, R0");
    CHECK(tokens[0] == "MOV");
    CHECK(tokens[1] == "$1");
    CHECK(tokens[2] == "R0");

    tokens = Tokenize("MOV $1,R0");
    CHECK(tokens[0] == "MOV");
    CHECK(tokens[1] == "$1");
    CHECK(tokens[2] == "R0");
    
    tokens = Tokenize("MOV $123456789, R6");
    CHECK(tokens[0] == "MOV");
    CHECK(tokens[1] == "$123456789");
    CHECK(tokens[2] == "R6");

    tokens = Tokenize("MOV $123456789 R6");
    CHECK(tokens[0] == "MOV");
    CHECK(tokens[1] == "$123456789");
    CHECK(tokens[2] == "R6");
    
    tokens = Tokenize("MOV $123456789,R6");
    CHECK(tokens[0] == "MOV");
    CHECK(tokens[1] == "$123456789");
    CHECK(tokens[2] == "R6");

    tokens = Tokenize("MOV R1,R6");
    CHECK(tokens[0] == "MOV");
    CHECK(tokens[1] == "R1");
    CHECK(tokens[2] == "R6");

    tokens = Tokenize("JMP R0");
    CHECK(tokens[0] == "JMP");
    CHECK(tokens[1] == "R0");

    tokens = Tokenize("HLT");
    CHECK(tokens[0] == "HLT");

    tokens = Tokenize("HLT");
    CHECK(tokens[0] == "HLT");

    SUBCASE("Not allowed number of tokens")
    {
        // Not the tokenizers responsibility to report this so this is a correct test
        tokens = Tokenize("MOV $1, R0, R1");
        CHECK(tokens[0] == "MOV");
        CHECK(tokens[1] == "$1");
        CHECK(tokens[2] == "R0");
        CHECK(tokens[3] == "R1");
    }
}


TEST_CASE("Testing ParseLine()")
{
    SUBCASE("Non instruction lines")
    {
        auto ins = ParseLine("# Test comment", 0);
        CHECK(ins.has_value() == false);

        ins = ParseLine("          # Test comment", 0);
        CHECK(ins.has_value() == false);

        ins = ParseLine("          ", 0);
        CHECK(ins.has_value() == false);

        ins = ParseLine("IsEqual: TokenAfterLabelError", 0);
        CHECK(ins.has_value() == false);
        // IsEqual:TokenAfterLabelError Read TODO in Parser.cpp of Assembler-Lib
    }


    SUBCASE("Instruction lines")
    {
        auto ins = ParseLine("IsEqual:", 0).value();
        CHECK(ins.lineNumber == 0);
        CHECK(ins.label      == "IsEqual");
        CHECK(ins.opcode     == "");
        CHECK(ins.lhs        == "");
        CHECK(ins.rhs        == "");
        
        ins = ParseLine("   IsEqual:   ", 0).value();
        CHECK(ins.lineNumber == 0);
        CHECK(ins.label      == "IsEqual");
        CHECK(ins.opcode     == "");
        CHECK(ins.lhs        == "");
        CHECK(ins.rhs        == "");

        ins = ParseLine("   IsEqual:TokenAfterLabelError   ", 0xFF).value();
        CHECK(ins.lineNumber == 0xFF);
        CHECK(ins.label      == "");
        CHECK(ins.opcode     == "IsEqual:TokenAfterLabelError");
        CHECK(ins.lhs        == "");
        CHECK(ins.rhs        == "");

        ins = ParseLine("   IsEqual:TokenAfterLabelError   LHS RHS", 0xFF).value();
        CHECK(ins.lineNumber == 0xFF);
        CHECK(ins.label      == "");
        CHECK(ins.opcode     == "IsEqual:TokenAfterLabelError");
        CHECK(ins.lhs        == "LHS");
        CHECK(ins.rhs        == "RHS");

        ins = ParseLine("MOV $0xFF, R0", 0xFF).value();
        CHECK(ins.lineNumber == 0xFF);
        CHECK(ins.label      == "");
        CHECK(ins.opcode     == "MOV");
        CHECK(ins.lhs        == "$0xFF");
        CHECK(ins.rhs        == "R0");

        ins = ParseLine("MOV $0xFF,R0", 0xFF).value();
        CHECK(ins.lineNumber == 0xFF);
        CHECK(ins.label      == "");
        CHECK(ins.opcode     == "MOV");
        CHECK(ins.lhs        == "$0xFF");
        CHECK(ins.rhs        == "R0");

        ins = ParseLine("MOV $0xFF R0", 0xFF).value();
        CHECK(ins.lineNumber == 0xFF);
        CHECK(ins.label      == "");
        CHECK(ins.opcode     == "MOV");
        CHECK(ins.lhs        == "$0xFF");
        CHECK(ins.rhs        == "R0");
    }


    SUBCASE("Too many operands")
    {
        auto ins = ParseLine("MOV $0xFF, R0, R1", 0xFF);
        CHECK(ins.has_value() == false);

        ins = ParseLine("MOV $0xFF, R0, R1, R2", 0xFF);
        CHECK(ins.has_value() == false);

        ins = ParseLine("MOV $0xFF R0 R1 R2", 0xFF);
        CHECK(ins.has_value() == false);

        ins = ParseLine("MOV $0xFF,R0, R1, R2", 0xFF);
        CHECK(ins.has_value() == false);

        ins = ParseLine("MOV $0xFF,R0,R1,R2", 0xFF);
        CHECK(ins.has_value() == true);
        CHECK(ins.value().lineNumber == 0xFF);
        CHECK(ins.value().label      == "");
        CHECK(ins.value().opcode     == "MOV");
        CHECK(ins.value().lhs        == "$0xFF");
        CHECK(ins.value().rhs        == "R0,R1,R2");
    }


    SUBCASE("Not enought operands")
    {
        auto ins = ParseLine("MOV $0xFF,", 0xFF).value();
        CHECK(ins.lineNumber == 0xFF);
        CHECK(ins.label      == "");
        CHECK(ins.opcode     == "MOV");
        CHECK(ins.lhs        == "$0xFF");
        CHECK(ins.rhs        == "");

        ins = ParseLine("MOV $0xFF", 0xFF).value();
        CHECK(ins.lineNumber == 0xFF);
        CHECK(ins.label      == "");
        CHECK(ins.opcode     == "MOV");
        CHECK(ins.lhs        == "$0xFF");
        CHECK(ins.rhs        == "");

        ins = ParseLine("MOV", 0xFF).value();
        CHECK(ins.lineNumber == 0xFF);
        CHECK(ins.label      == "");
        CHECK(ins.opcode     == "MOV");
        CHECK(ins.lhs        == "");
        CHECK(ins.rhs        == "");
    }
}