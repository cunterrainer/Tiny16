#include "doctest/doctest.h"

#include "Parser.hpp"

#include "Utility/Result.hpp"

TEST_CASE("Testing ParseLine() Instructions")
{
    SUBCASE("Instructions not allowed")
    {
        CHECK(ParseLine("MOV R1 R0"     , 0xFF).IsErr()); // Missing comma between operands
        CHECK(ParseLine("MOV, R1 R0"    , 0xFF).IsErr()); // Comma immediately after opcode without space (invalid syntax)
        CHECK(ParseLine("MOV, R1, R0"   , 0xFF).IsErr()); // Comma immediately after opcode with operands (still invalid)
        CHECK(ParseLine("MOV R1 R0 R1"  , 0xFF).IsErr()); // Too many operands (3 operands, only max 2 allowed)
        CHECK(ParseLine("MOV R1 R0, R1" , 0xFF).IsErr()); // Too many operands with comma misplaced
        CHECK(ParseLine("MOV R1, R0, R1", 0xFF).IsErr()); // Too many operands with two commas (3 operands)
        CHECK(ParseLine("___ R1, R2"    , 0xFF).IsErr()); // Invalid opcode name with underscores (only letters allowed)
        CHECK(ParseLine("1234 R1, R2"   , 0xFF).IsErr()); // Opcode is numeric, which is invalid
        CHECK(ParseLine("MOV,R1,R0"     , 0xFF).IsErr()); // Comma immediately after opcode with no space (invalid)
        CHECK(ParseLine("MOV R1,, R0"   , 0xFF).IsErr()); // Double commas without operand in between
        CHECK(ParseLine("MOV , R2"      , 0xFF).IsErr()); // Missing first operand (empty before comma)
        CHECK(ParseLine("MOV R1, "      , 0xFF).IsErr()); // Missing second operand (empty after comma)
        CHECK(ParseLine("MO!V R1, R2"   , 0xFF).IsErr()); // Opcode contains invalid character '!' (only letters allowed)
        CHECK(ParseLine("MOV R1, R2,"   , 0xFF).IsErr()); // Trailing comma with no operand after
        CHECK(ParseLine("MOV , , R2"    , 0xFF).IsErr()); // Multiple commas in a row with empty operands
    }


    SUBCASE("Instructions allowed")
    {
        auto ins = ParseLine("MOV R1, R0", 0xFF);
        CHECK(ins.IsOk() == true);
        CHECK(ins.Ok().label == "");
        CHECK(ins.Ok().opcode == "MOV");
        CHECK(ins.Ok().lhs == "R1");
        CHECK(ins.Ok().rhs == "R0");
        CHECK(ins.Ok().lineNumber == 0xFF);
    
        ins = ParseLine("MOV R1", 0xFF);
        CHECK(ins.IsOk() == true);
        CHECK(ins.Ok().label == "");
        CHECK(ins.Ok().opcode == "MOV");
        CHECK(ins.Ok().lhs == "R1");
        CHECK(ins.Ok().rhs == "");
        CHECK(ins.Ok().lineNumber == 0xFF);
    
        ins = ParseLine("MOV $9090, $9090", 0xFF);
        CHECK(ins.IsOk() == true);
        CHECK(ins.Ok().label == "");
        CHECK(ins.Ok().opcode == "MOV");
        CHECK(ins.Ok().lhs == "$9090");
        CHECK(ins.Ok().rhs == "$9090");
        CHECK(ins.Ok().lineNumber == 0xFF);
    
        ins = ParseLine("MOV $4, R0", 0xFF);
        CHECK(ins.IsOk() == true);
        CHECK(ins.Ok().label == "");
        CHECK(ins.Ok().opcode == "MOV");
        CHECK(ins.Ok().lhs == "$4");
        CHECK(ins.Ok().rhs == "R0");
        CHECK(ins.Ok().lineNumber == 0xFF);
    
        ins = ParseLine("MOV $0b111, R1", 0xFF);
        CHECK(ins.IsOk() == true);
        CHECK(ins.Ok().label == "");
        CHECK(ins.Ok().opcode == "MOV");
        CHECK(ins.Ok().lhs == "$0b111");
        CHECK(ins.Ok().rhs == "R1");
        CHECK(ins.Ok().lineNumber == 0xFF);
    
        ins = ParseLine("MOV $0x00FF, R1", 0xFF);
        CHECK(ins.IsOk() == true);
        CHECK(ins.Ok().label == "");
        CHECK(ins.Ok().opcode == "MOV");
        CHECK(ins.Ok().lhs == "$0x00FF");
        CHECK(ins.Ok().rhs == "R1");
        CHECK(ins.Ok().lineNumber == 0xFF);
    
        ins = ParseLine("JMP R1", 0xFF);
        CHECK(ins.IsOk() == true);
        CHECK(ins.Ok().label == "");
        CHECK(ins.Ok().opcode == "JMP");
        CHECK(ins.Ok().lhs == "R1");
        CHECK(ins.Ok().rhs == "");
        CHECK(ins.Ok().lineNumber == 0xFF);
    
        ins = ParseLine("MOV", 0xFF);
        CHECK(ins.IsOk() == true);
        CHECK(ins.Ok().label == "");
        CHECK(ins.Ok().opcode == "MOV");
        CHECK(ins.Ok().lhs == "");
        CHECK(ins.Ok().rhs == "");
        CHECK(ins.Ok().lineNumber == 0xFF);
    
        ins = ParseLine("   HLT   ", 0xFF);
        CHECK(ins.IsOk() == true);
        CHECK(ins.Ok().label == "");
        CHECK(ins.Ok().opcode == "HLT");
        CHECK(ins.Ok().lhs == "");
        CHECK(ins.Ok().rhs == "");
        CHECK(ins.Ok().lineNumber == 0xFF);
    
        ins = ParseLine("   SUB R1, R2   ", 0xFF);
        CHECK(ins.IsOk() == true);
        CHECK(ins.Ok().label == "");
        CHECK(ins.Ok().opcode == "SUB");
        CHECK(ins.Ok().lhs == "R1");
        CHECK(ins.Ok().rhs == "R2");
        CHECK(ins.Ok().lineNumber == 0xFF);
    }
}


TEST_CASE("Testing ParseLine() Labels and Commas")
{
    SUBCASE("Non instruction lines")
    {
        auto ins = ParseLine("# Test comment", 0);
        CHECK(ins.IsOk());
        CHECK(ins.Ok().label.empty());
        CHECK(ins.Ok().opcode.empty());

        ins = ParseLine("          # Test comment", 0);
        CHECK(ins.IsOk());
        CHECK(ins.Ok().label.empty());
        CHECK(ins.Ok().opcode.empty());
        
        ins = ParseLine("          ", 0);
        CHECK(ins.IsOk());
        CHECK(ins.Ok().label.empty());
        CHECK(ins.Ok().opcode.empty());
    }
    
    
    SUBCASE("Labels not allowed")
    {
        CHECK(ParseLine("IsEqual: Test"       , 0).IsErr());
        CHECK(ParseLine("IsEqual:Test"        , 0).IsErr());
        CHECK(ParseLine("123IsEqual: Test"    , 0).IsErr());
        CHECK(ParseLine("123IsEqual:Test"     , 0).IsErr());
        CHECK(ParseLine("   IsEqual:  Test   ", 0).IsErr());
    }
    
    
    SUBCASE("Labels allowed")
    {
        auto ins = ParseLine("IsEqual:", 0);
        CHECK(ins.IsOk() == true);
        CHECK(ins.Ok().label  == "IsEqual");
        CHECK(ins.Ok().opcode == "");
        CHECK(ins.Ok().lhs    == "");
        CHECK(ins.Ok().rhs    == "");
        CHECK(ins.Ok().lineNumber == 0);
    
        ins = ParseLine("IsEqual:    ", 0);
        CHECK(ins.IsOk() == true);
        CHECK(ins.Ok().label  == "IsEqual");
        CHECK(ins.Ok().opcode == "");
        CHECK(ins.Ok().lhs    == "");
        CHECK(ins.Ok().rhs    == "");
        CHECK(ins.Ok().lineNumber == 0);
    
        ins = ParseLine("IsEqual:  # This is a comma", 0);
        CHECK(ins.IsOk() == true);
        CHECK(ins.Ok().label  == "IsEqual");
        CHECK(ins.Ok().opcode == "");
        CHECK(ins.Ok().lhs    == "");
        CHECK(ins.Ok().rhs    == "");
        CHECK(ins.Ok().lineNumber == 0);
    
        ins = ParseLine("   _IsEqual:   # This is a comma", 0);
        CHECK(ins.IsOk() == true);
        CHECK(ins.Ok().label  == "_IsEqual");
        CHECK(ins.Ok().opcode == "");
        CHECK(ins.Ok().lhs    == "");
        CHECK(ins.Ok().rhs    == "");
        CHECK(ins.Ok().lineNumber == 0);
    
        ins = ParseLine("   _Is_Equal:   # This is a comma", 0);
        CHECK(ins.IsOk() == true);
        CHECK(ins.Ok().label  == "_Is_Equal");
        CHECK(ins.Ok().opcode == "");
        CHECK(ins.Ok().lhs    == "");
        CHECK(ins.Ok().rhs    == "");
        CHECK(ins.Ok().lineNumber == 0);
    }
}