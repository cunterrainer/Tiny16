#include "doctest/doctest.h"

#include "Parser.hpp"
#include "Validator.hpp"

#include "Utility/Result.hpp"

TEST_CASE("Test ValidateInstruction()")
{
    SUBCASE("Invalid instruction")
    {
        // HLT should not have any operands
        CHECK_FALSE(ValidateInstruction(ParseLine("HLT R1", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("HLT R1, R2", 0).Ok()).valid);

        // JMP/JE require exactly one operand
        CHECK_FALSE(ValidateInstruction(ParseLine("JMP", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("JE", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("JMP R1, R2", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("JE R1, R2", 0).Ok()).valid);

        // Destination must be a valid register
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV R1, $4", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV $0x6, $0x7", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV $0x4, AA", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV 0x4, R1", 0).Ok()).valid);    // missing $
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV $5, $6", 0).Ok()).valid);     // both immediates
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV R8, R1", 0).Ok()).valid);     // R8 invalid if only R0-R7 allowed
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV $+4, R9", 0).Ok()).valid);    // R9 invalid
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV $-2, X1", 0).Ok()).valid);    // invalid register name

        // Source must be a valid register or immediate
        CHECK_FALSE(ValidateInstruction(ParseLine("ADD foo, R1", 0).Ok()).valid);    // invalid source literal
        CHECK_FALSE(ValidateInstruction(ParseLine("SUB #4, R1", 0).Ok()).valid);     // unsupported immediate syntax
        CHECK_FALSE(ValidateInstruction(ParseLine("CMP 0x10, R2", 0).Ok()).valid);   // hex without $
        CHECK_FALSE(ValidateInstruction(ParseLine("CMP $0z10, R2", 0).Ok()).valid);  // malformed hex
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV $, R1", 0).Ok()).valid);      // empty immediate

        CHECK_FALSE(ValidateInstruction(ParseLine("MOV  $0b4, R0", 0).Ok()).valid);  // 0b4 not valid binary
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV $+0B4, R7", 0).Ok()).valid);  // 0b4 not valid binary
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV $+0bFF, R7", 0).Ok()).valid); // 0bFF not valid binary
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV $-0b01, R7", 0).Ok()).valid); // negative binary not allowed

        // Completely malformed operands
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV R1, $", 0).Ok()).valid);      // empty immediate value
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV $, $", 0).Ok()).valid);       // both sides empty
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV $-0xZZ, R1", 0).Ok()).valid); // malformed signed hex

        // Source must be valid (either immediate or register)
        CHECK_FALSE(ValidateInstruction(ParseLine("ADD 4, R1", 0).Ok()).valid);   // missing $

        // Both operands missing
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("ADD", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("SUB", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("CMP", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("LOAD", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("STORE", 0).Ok()).valid);

        // Only one operand
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV R1", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("ADD R1", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("SUB R1", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("CMP R1", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("LOAD R1", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("STORE R1", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV $1", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("ADD $1", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("SUB $1", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("CMP $1", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("LOAD $1", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("STORE $1", 0).Ok()).valid);

        // Invalid opcode
        CHECK_FALSE(ValidateInstruction(ParseLine("XYZ R1, R2", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("XYZ R1", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("XYZ", 0).Ok()).valid);
        
        // Load op1 has to be an address and op2 ahs to be a register
        CHECK_FALSE(ValidateInstruction(ParseLine("LOAD R1, R7", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("LOAD R1, R7", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("LOAD R1, R0", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("LOAD R1, $3", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("LOAD R1, $0x20", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("LOAD R1, $0b11", 0).Ok()).valid);

        // Store op1 has to be a register and op2 has to be an address
        CHECK_FALSE(ValidateInstruction(ParseLine("STORE  $0x4, R0", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("STORE $+0x4, R7", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("STORE $-0x4, R7", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("STORE R1, R0", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("STORE R1, R7", 0).Ok()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("STORE R1, R7", 0).Ok()).valid);
    }


    SUBCASE("Valid Instruction")
    {
        CHECK(ValidateInstruction(ParseLine("HLT", 0).Ok()).valid);
        
        CHECK(ValidateInstruction(ParseLine("JMP Label", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("JE  Label", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("JMP 0x99", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("JE  0x99", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("JMP R1", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("JE  R1", 0).Ok()).valid);
        
        CHECK(ValidateInstruction(ParseLine("MOV  $0x4, R0", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("MOV $+0X4, R7", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("MOV $-0x4, R7", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("MOV R1, R0", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("MOV R1, R7", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("MOV R1, R7", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("MOV  $0b101, R0", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("MOV $+0B101, R7", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("MOV  $999, R0", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("MOV $+999, R7", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("MOV $-999, R7", 0).Ok()).valid);
        
        CHECK(ValidateInstruction(ParseLine("ADD  $0x4, R0", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("ADD $+0x4, R7", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("ADD $-0x4, R7", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("ADD R1, R0", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("ADD R1, R7", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("ADD R1, R7", 0).Ok()).valid);
        
        CHECK(ValidateInstruction(ParseLine("SUB  $0x4, R0", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("SUB $+0x4, R7", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("SUB $-0x4, R7", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("SUB R1, R0", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("SUB R1, R7", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("SUB R1, R7", 0).Ok()).valid);
        
        CHECK(ValidateInstruction(ParseLine("CMP  $0x4, R0", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("CMP $+0x4, R7", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("CMP $-0x4, R7", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("CMP R1, R0", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("CMP R1, R7", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("CMP R1, R7", 0).Ok()).valid);
        
        CHECK(ValidateInstruction(ParseLine("LOAD  $0x4, R0", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("LOAD $+0x4, R7", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("LOAD $-0x4, R7", 0).Ok()).valid);
        
        CHECK(ValidateInstruction(ParseLine("STORE R0, $0xFF", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("STORE R7, $0x29", 0).Ok()).valid);
        CHECK(ValidateInstruction(ParseLine("STORE R7, $0b11", 0).Ok()).valid);
    }
}


bool IsValidRegister(std::string_view s);
TEST_CASE("Test IsValidRegister()")
{
    SUBCASE("Invalid")
    {
        CHECK_FALSE(IsValidRegister("R"));
        CHECK_FALSE(IsValidRegister("AA"));
        CHECK_FALSE(IsValidRegister(""));
        CHECK_FALSE(IsValidRegister(" "));
        CHECK_FALSE(IsValidRegister("RR"));
        CHECK_FALSE(IsValidRegister("R8"));
        CHECK_FALSE(IsValidRegister("R-1"));
        CHECK_FALSE(IsValidRegister("R9"));
    }
    
    
    SUBCASE("Valid")
    {
        CHECK(IsValidRegister("R0"));
        CHECK(IsValidRegister("R1"));
        CHECK(IsValidRegister("R2"));
        CHECK(IsValidRegister("R3"));
        CHECK(IsValidRegister("R4"));
        CHECK(IsValidRegister("R5"));
        CHECK(IsValidRegister("R6"));
        CHECK(IsValidRegister("R7"));
        
        CHECK(IsValidRegister("r0"));
        CHECK(IsValidRegister("r1"));
        CHECK(IsValidRegister("r2"));
        CHECK(IsValidRegister("r3"));
        CHECK(IsValidRegister("r4"));
        CHECK(IsValidRegister("r5"));
        CHECK(IsValidRegister("r6"));
        CHECK(IsValidRegister("r7"));
    }
}


bool IsValidImmediate(std::string_view s);
TEST_CASE("IsValidImmediatie()")
{
    SUBCASE("Invalid")
    {
        // Empty or whitespace
        CHECK_FALSE(IsValidImmediate(""));
        CHECK_FALSE(IsValidImmediate(" "));
        CHECK_FALSE(IsValidImmediate("  "));

        // Not starting with $
        CHECK_FALSE(IsValidImmediate("R"));
        CHECK_FALSE(IsValidImmediate("R1"));
        CHECK_FALSE(IsValidImmediate("1"));
        CHECK_FALSE(IsValidImmediate("99"));
        CHECK_FALSE(IsValidImmediate("999"));
        CHECK_FALSE(IsValidImmediate("0x99"));
        CHECK_FALSE(IsValidImmediate("0x"));
        CHECK_FALSE(IsValidImmediate("0x9"));
        CHECK_FALSE(IsValidImmediate("0b01"));
        CHECK_FALSE(IsValidImmediate("-0b01"));

        // Malformed dollar-prefixed
        CHECK_FALSE(IsValidImmediate("$"));             // only $
        CHECK_FALSE(IsValidImmediate("$ "));            // $ with space
        CHECK_FALSE(IsValidImmediate("$-"));            // $ minus nothing
        CHECK_FALSE(IsValidImmediate("$--1"));          // double minus
        CHECK_FALSE(IsValidImmediate("$++1"));          // double plus
        CHECK_FALSE(IsValidImmediate("$+-1"));          // invalid mixed signs
        CHECK_FALSE(IsValidImmediate("$-0b01"));        // binary cannot be signed
        CHECK_FALSE(IsValidImmediate("$0x"));           // no digits
        CHECK_FALSE(IsValidImmediate("$0xGHI"));        // invalid hex
        CHECK_FALSE(IsValidImmediate("$0b"));           // no bits
        CHECK_FALSE(IsValidImmediate("$0b012"));        // invalid binary
        CHECK_FALSE(IsValidImmediate("$0b2"));          // binary with '2'
        CHECK_FALSE(IsValidImmediate("$0b00a"));        // binary with letter
        CHECK_FALSE(IsValidImmediate("$0x123Z"));       // invalid hex tail
        CHECK_FALSE(IsValidImmediate("$10A"));          // decimal with letter
        CHECK_FALSE(IsValidImmediate("$10.1"));         // floats not supported
        CHECK_FALSE(IsValidImmediate("$+"));            // incomplete
        CHECK_FALSE(IsValidImmediate("$-"));            // incomplete
        CHECK_FALSE(IsValidImmediate("$-0x"));          // no hex digits
        CHECK_FALSE(IsValidImmediate("$+0b"));          // no binary digits
        CHECK_FALSE(IsValidImmediate("$+0xG"));         // invalid hex digit
        CHECK_FALSE(IsValidImmediate("$0x+4"));         // misplaced sign
        CHECK_FALSE(IsValidImmediate("$+00xF"));        // invalid hex digit
        CHECK_FALSE(IsValidImmediate("$000b11"));       // invalid binary digit

        // Misplaced sign or multiple signs
        CHECK_FALSE(IsValidImmediate("$++0x4"));
        CHECK_FALSE(IsValidImmediate("$--0x4"));
        CHECK_FALSE(IsValidImmediate("$0x-4"));  // sign must come before 0x
    }
    
    
    SUBCASE("Valid")
    {
        // Decimal
        CHECK(IsValidImmediate("$0"));
        CHECK(IsValidImmediate("$1"));
        CHECK(IsValidImmediate("$42"));
        CHECK(IsValidImmediate("$+123"));
        CHECK(IsValidImmediate("$-999"));
        CHECK(IsValidImmediate("$000123"));

        // Hexadecimal
        CHECK(IsValidImmediate("$0x0"));
        CHECK(IsValidImmediate("$0x1"));
        CHECK(IsValidImmediate("$0xFF"));
        CHECK(IsValidImmediate("$0XdeadBEEF"));
        CHECK(IsValidImmediate("$+0xABC"));
        CHECK(IsValidImmediate("$-0x10"));

        // Binary
        CHECK(IsValidImmediate("$0b0"));
        CHECK(IsValidImmediate("$0b1"));
        CHECK(IsValidImmediate("$0b01"));
        CHECK(IsValidImmediate("$0B101010"));
        CHECK(IsValidImmediate("$+0b11"));  // '+' allowed even though not meaningful
        CHECK(IsValidImmediate("$0b00001111"));

        // Edge cases
        CHECK(IsValidImmediate("$+0"));
        CHECK(IsValidImmediate("$-0"));
        CHECK(IsValidImmediate("$+0x0"));
        CHECK(IsValidImmediate("$-0x0"));

        // Upper and lower case consistency
        CHECK(IsValidImmediate("$0XFF"));
        CHECK(IsValidImmediate("$0xFF"));
        CHECK(IsValidImmediate("$0B10"));
        CHECK(IsValidImmediate("$0b10"));
    }
}