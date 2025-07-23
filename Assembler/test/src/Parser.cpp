#include "doctest/doctest.h"

#include "Parser.hpp"

TEST_CASE("Testing ParseLine() Instructions")
{
    SUBCASE("Instructions not allowed")
    {
        // Missing comma between operands
        auto ins = ParseLine("MOV R1 R0", 0xFF);
        CHECK(ins.has_value() == false);

        // Comma immediately after opcode without space (invalid syntax)
        ins = ParseLine("MOV, R1 R0", 0xFF);
        CHECK(ins.has_value() == false);

        // Comma immediately after opcode with operands (still invalid)
        ins = ParseLine("MOV, R1, R0", 0xFF);
        CHECK(ins.has_value() == false);

        // Too many operands (3 operands, only max 2 allowed)
        ins = ParseLine("MOV R1 R0 R1", 0xFF);
        CHECK(ins.has_value() == false);

        // Too many operands with comma misplaced
        ins = ParseLine("MOV R1 R0, R1", 0xFF);
        CHECK(ins.has_value() == false);

        // Too many operands with two commas (3 operands)
        ins = ParseLine("MOV R1, R0, R1", 0xFF);
        CHECK(ins.has_value() == false);

        // Invalid opcode name with underscores (only letters allowed)
        ins = ParseLine("___ R1, R2", 0xFF);
        CHECK(ins.has_value() == false);

        // Opcode is numeric, which is invalid
        ins = ParseLine("1234 R1, R2", 0xFF);
        CHECK(ins.has_value() == false);

        // Comma immediately after opcode with no space (invalid)
        ins = ParseLine("MOV,R1,R0", 0xFF);
        CHECK(ins.has_value() == false);

        // Double commas without operand in between
        ins = ParseLine("MOV R1,, R0", 0xFF);
        CHECK(ins.has_value() == false);

        // Missing first operand (empty before comma)
        ins = ParseLine("MOV , R2", 0xFF);
        CHECK(ins.has_value() == false);

        // Missing second operand (empty after comma)
        ins = ParseLine("MOV R1, ", 0xFF);
        CHECK(ins.has_value() == false);

        // Opcode contains invalid character '!' (only letters allowed)
        ins = ParseLine("MO!V R1, R2", 0xFF);
        CHECK(ins.has_value() == false);

        // Trailing comma with no operand after
        ins = ParseLine("MOV R1, R2,", 0xFF);
        CHECK(ins.has_value() == false);

        // Multiple commas in a row with empty operands
        ins = ParseLine("MOV , , R2", 0xFF);
        CHECK(ins.has_value() == false);
    }


    SUBCASE("Instructions allowed")
    {
        auto ins = ParseLine("MOV R1, R0", 0xFF);
        CHECK(ins.has_value() == true);
        CHECK(ins.value().label == "");
        CHECK(ins.value().opcode == "MOV");
        CHECK(ins.value().lhs == "R1");
        CHECK(ins.value().rhs == "R0");
        CHECK(ins.value().lineNumber == 0xFF);

        ins = ParseLine("MOV R1", 0xFF);
        CHECK(ins.has_value() == true);
        CHECK(ins.value().label == "");
        CHECK(ins.value().opcode == "MOV");
        CHECK(ins.value().lhs == "R1");
        CHECK(ins.value().rhs == "");
        CHECK(ins.value().lineNumber == 0xFF);

        ins = ParseLine("MOV $9090, $9090", 0xFF);
        CHECK(ins.has_value() == true);
        CHECK(ins.value().label == "");
        CHECK(ins.value().opcode == "MOV");
        CHECK(ins.value().lhs == "$9090");
        CHECK(ins.value().rhs == "$9090");
        CHECK(ins.value().lineNumber == 0xFF);

        ins = ParseLine("MOV $4, R0", 0xFF);
        CHECK(ins.has_value() == true);
        CHECK(ins.value().label == "");
        CHECK(ins.value().opcode == "MOV");
        CHECK(ins.value().lhs == "$4");
        CHECK(ins.value().rhs == "R0");
        CHECK(ins.value().lineNumber == 0xFF);

        ins = ParseLine("MOV $0b111, R1", 0xFF);
        CHECK(ins.has_value() == true);
        CHECK(ins.value().label == "");
        CHECK(ins.value().opcode == "MOV");
        CHECK(ins.value().lhs == "$0b111");
        CHECK(ins.value().rhs == "R1");
        CHECK(ins.value().lineNumber == 0xFF);

        ins = ParseLine("MOV $0x00FF, R1", 0xFF);
        CHECK(ins.has_value() == true);
        CHECK(ins.value().label == "");
        CHECK(ins.value().opcode == "MOV");
        CHECK(ins.value().lhs == "$0x00FF");
        CHECK(ins.value().rhs == "R1");
        CHECK(ins.value().lineNumber == 0xFF);

        ins = ParseLine("JMP R1", 0xFF);
        CHECK(ins.has_value() == true);
        CHECK(ins.value().label == "");
        CHECK(ins.value().opcode == "JMP");
        CHECK(ins.value().lhs == "R1");
        CHECK(ins.value().rhs == "");
        CHECK(ins.value().lineNumber == 0xFF);

        ins = ParseLine("MOV", 0xFF);
        CHECK(ins.has_value() == true);
        CHECK(ins.value().label == "");
        CHECK(ins.value().opcode == "MOV");
        CHECK(ins.value().lhs == "");
        CHECK(ins.value().rhs == "");
        CHECK(ins.value().lineNumber == 0xFF);

        ins = ParseLine("   HLT   ", 0xFF);
        CHECK(ins.has_value() == true);
        CHECK(ins.value().label == "");
        CHECK(ins.value().opcode == "HLT");
        CHECK(ins.value().lhs == "");
        CHECK(ins.value().rhs == "");
        CHECK(ins.value().lineNumber == 0xFF);

        ins = ParseLine("   SUB R1, R2   ", 0xFF);
        CHECK(ins.has_value() == true);
        CHECK(ins.value().label == "");
        CHECK(ins.value().opcode == "SUB");
        CHECK(ins.value().lhs == "R1");
        CHECK(ins.value().rhs == "R2");
        CHECK(ins.value().lineNumber == 0xFF);
    }
}


TEST_CASE("Testing ParseLine() Labels and Commas")
{
    SUBCASE("Non instruction lines")
    {
        auto ins = ParseLine("# Test comment", 0);
        CHECK(ins.has_value() == false);
    
        ins = ParseLine("          # Test comment", 0);
        CHECK(ins.has_value() == false);
    
        ins = ParseLine("          ", 0);
        CHECK(ins.has_value() == false);
    }


    SUBCASE("Labels not allowed")
    {
        auto ins = ParseLine("IsEqual: Test", 0);
        CHECK(ins.has_value() == false);

        ins = ParseLine("IsEqual:Test", 0);
        CHECK(ins.has_value() == false);

        ins = ParseLine("123IsEqual: Test", 0);
        CHECK(ins.has_value() == false);

        ins = ParseLine("123IsEqual:Test", 0);
        CHECK(ins.has_value() == false);

        ins = ParseLine("   IsEqual:  Test   ", 0);
        CHECK(ins.has_value() == false);
    }


    SUBCASE("Labels allowed")
    {
        auto ins = ParseLine("IsEqual:", 0);
        CHECK(ins.has_value() == true);
        CHECK(ins.value().label  == "IsEqual");
        CHECK(ins.value().opcode == "");
        CHECK(ins.value().lhs    == "");
        CHECK(ins.value().rhs    == "");
        CHECK(ins.value().lineNumber == 0);

        ins = ParseLine("IsEqual:    ", 0);
        CHECK(ins.has_value() == true);
        CHECK(ins.value().label  == "IsEqual");
        CHECK(ins.value().opcode == "");
        CHECK(ins.value().lhs    == "");
        CHECK(ins.value().rhs    == "");
        CHECK(ins.value().lineNumber == 0);

        ins = ParseLine("IsEqual:  # This is a comma", 0);
        CHECK(ins.has_value() == true);
        CHECK(ins.value().label  == "IsEqual");
        CHECK(ins.value().opcode == "");
        CHECK(ins.value().lhs    == "");
        CHECK(ins.value().rhs    == "");
        CHECK(ins.value().lineNumber == 0);

        ins = ParseLine("   _IsEqual:   # This is a comma", 0);
        CHECK(ins.has_value() == true);
        CHECK(ins.value().label  == "_IsEqual");
        CHECK(ins.value().opcode == "");
        CHECK(ins.value().lhs    == "");
        CHECK(ins.value().rhs    == "");
        CHECK(ins.value().lineNumber == 0);

        ins = ParseLine("   _Is_Equal:   # This is a comma", 0);
        CHECK(ins.has_value() == true);
        CHECK(ins.value().label  == "_Is_Equal");
        CHECK(ins.value().opcode == "");
        CHECK(ins.value().lhs    == "");
        CHECK(ins.value().rhs    == "");
        CHECK(ins.value().lineNumber == 0);
    }
}