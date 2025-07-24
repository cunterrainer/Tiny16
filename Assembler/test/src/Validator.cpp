#include "doctest/doctest.h"

#include "Parser.hpp"
#include "Validator.hpp"

TEST_CASE("Test ValidateInstruction()")
{
    SUBCASE("Invalid instruction")
    {
        // HLT should not have any operands
        CHECK_FALSE(ValidateInstruction(ParseLine("HLT R1", 0).value()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("HLT R1, R2", 0).value()).valid);

        // JMP/JE require exactly one operand
        CHECK_FALSE(ValidateInstruction(ParseLine("JMP", 0).value()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("JE", 0).value()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("JMP R1, R2", 0).value()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("JE R1, R2", 0).value()).valid);

        // Destination must be a valid register
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV R1, $4", 0).value()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV $0x6, $0x7", 0).value()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV $0x4, AA", 0).value()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV 0x4, R1", 0).value()).valid);    // missing $
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV $5, $6", 0).value()).valid);     // both immediates
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV R8, R1", 0).value()).valid);     // R8 invalid if only R0-R7 allowed
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV $+4, R9", 0).value()).valid);    // R9 invalid
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV $-2, X1", 0).value()).valid);    // invalid register name

        // Source must be a valid register or immediate
        CHECK_FALSE(ValidateInstruction(ParseLine("ADD foo, R1", 0).value()).valid);    // invalid source literal
        CHECK_FALSE(ValidateInstruction(ParseLine("SUB #4, R1", 0).value()).valid);     // unsupported immediate syntax
        CHECK_FALSE(ValidateInstruction(ParseLine("CMP 0x10, R2", 0).value()).valid);   // hex without $
        CHECK_FALSE(ValidateInstruction(ParseLine("CMP $0z10, R2", 0).value()).valid);  // malformed hex
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV $, R1", 0).value()).valid);      // empty immediate

        CHECK_FALSE(ValidateInstruction(ParseLine("MOV  $0b4, R0", 0).value()).valid);  // 0b4 not valid binary
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV $+0B4, R7", 0).value()).valid);  // 0b4 not valid binary
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV $+0bFF, R7", 0).value()).valid); // 0bFF not valid binary
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV $-0b01, R7", 0).value()).valid); // negative binary not allowed

        // Completely malformed operands
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV R1, $", 0).value()).valid);      // empty immediate value
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV $, $", 0).value()).valid);       // both sides empty
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV $-0xZZ, R1", 0).value()).valid); // malformed signed hex

        // Source must be valid (either immediate or register)
        CHECK_FALSE(ValidateInstruction(ParseLine("ADD 4, R1", 0).value()).valid);   // missing $

        // Both operands missing
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV", 0).value()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("ADD", 0).value()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("SUB", 0).value()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("CMP", 0).value()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("LOAD", 0).value()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("STORE", 0).value()).valid);

        // Only one operand
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV R1", 0).value()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("ADD R1", 0).value()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("SUB R1", 0).value()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("CMP R1", 0).value()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("LOAD R1", 0).value()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("STORE R1", 0).value()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("MOV $1", 0).value()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("ADD $1", 0).value()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("SUB $1", 0).value()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("CMP $1", 0).value()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("LOAD $1", 0).value()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("STORE $1", 0).value()).valid);

        // Invalid opcode
        CHECK_FALSE(ValidateInstruction(ParseLine("XYZ R1, R2", 0).value()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("XYZ R1", 0).value()).valid);
        CHECK_FALSE(ValidateInstruction(ParseLine("XYZ", 0).value()).valid);
    }


    SUBCASE("Valid Instruction")
    {
        CHECK(ValidateInstruction(ParseLine("HLT", 0).value()).valid);
        
        CHECK(ValidateInstruction(ParseLine("JMP Label", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("JE  Label", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("JMP 0x99", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("JE  0x99", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("JMP R1", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("JE  R1", 0).value()).valid);
        
        CHECK(ValidateInstruction(ParseLine("MOV  $0x4, R0", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("MOV $+0X4, R7", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("MOV $-0x4, R7", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("MOV R1, R0", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("MOV R1, R7", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("MOV R1, R7", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("MOV  $0b101, R0", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("MOV $+0B101, R7", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("MOV  $999, R0", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("MOV $+999, R7", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("MOV $-999, R7", 0).value()).valid);
        
        CHECK(ValidateInstruction(ParseLine("ADD  $0x4, R0", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("ADD $+0x4, R7", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("ADD $-0x4, R7", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("ADD R1, R0", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("ADD R1, R7", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("ADD R1, R7", 0).value()).valid);
        
        CHECK(ValidateInstruction(ParseLine("SUB  $0x4, R0", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("SUB $+0x4, R7", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("SUB $-0x4, R7", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("SUB R1, R0", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("SUB R1, R7", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("SUB R1, R7", 0).value()).valid);
        
        CHECK(ValidateInstruction(ParseLine("CMP  $0x4, R0", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("CMP $+0x4, R7", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("CMP $-0x4, R7", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("CMP R1, R0", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("CMP R1, R7", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("CMP R1, R7", 0).value()).valid);
        
        CHECK(ValidateInstruction(ParseLine("LOAD  $0x4, R0", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("LOAD $+0x4, R7", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("LOAD $-0x4, R7", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("LOAD R1, R0", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("LOAD R1, R7", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("LOAD R1, R7", 0).value()).valid);
        
        CHECK(ValidateInstruction(ParseLine("STORE  $0x4, R0", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("STORE $+0x4, R7", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("STORE $-0x4, R7", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("STORE R1, R0", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("STORE R1, R7", 0).value()).valid);
        CHECK(ValidateInstruction(ParseLine("STORE R1, R7", 0).value()).valid);
    }
}


bool IsValidImmediate(std::string_view s);
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