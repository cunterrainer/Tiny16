#include "doctest/doctest.h"

#include "Error.hpp"
#include "Parser.hpp"
#include "Validator.hpp"

#include "Utility/Result.hpp"

Result<void, ASMError> ValidateInstruction(const ParsedInstruction& instr, const std::unordered_set<std::string>& labels = {});
TEST_CASE("Test ValidateInstruction()")
{
    SUBCASE("Invalid instruction")
    {
        // HLT should not have any operands
        CHECK(ValidateInstruction(ParseLine("HALT R1", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("HALT R1, R2", 0).Ok()).IsErr());

        // JMP/JE require exactly one operand
        CHECK(ValidateInstruction(ParseLine("JMP", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("JE", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("JMP R1, R2", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("JE R1, R2", 0).Ok()).IsErr());

        // JMP/JE only accepts register or label
        CHECK(ValidateInstruction(ParseLine("JMP $0x99", 0).Ok(), { "123", "Test" }).IsErr());
        CHECK(ValidateInstruction(ParseLine("JE  $0x99", 0).Ok(), { "123", "Test" }).IsErr());

        // Destination must be a valid register
        CHECK(ValidateInstruction(ParseLine("MOV R1, $4", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("MOV $0x6, $0x7", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("MOV $0x4, AA", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("MOV 0x4, R1", 0).Ok()).IsErr());    // missing $
        CHECK(ValidateInstruction(ParseLine("MOV $5, $6", 0).Ok()).IsErr());     // both immediates
        CHECK(ValidateInstruction(ParseLine("MOV RZ, R1", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("MOV $+4, RU", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("MOV $-2, X1", 0).Ok()).IsErr());    // invalid register name

        // Source must be a valid register or immediate
        CHECK(ValidateInstruction(ParseLine("ADD foo, R1", 0).Ok()).IsErr());    // invalid source literal
        CHECK(ValidateInstruction(ParseLine("SUB #4, R1", 0).Ok()).IsErr());     // unsupported immediate syntax
        CHECK(ValidateInstruction(ParseLine("CMP 0x10, R2", 0).Ok()).IsErr());   // hex without $
        CHECK(ValidateInstruction(ParseLine("CMP $0z10, R2", 0).Ok()).IsErr());  // malformed hex
        CHECK(ValidateInstruction(ParseLine("MOV $, R1", 0).Ok()).IsErr());      // empty immediate

        CHECK(ValidateInstruction(ParseLine("MOV  $0b4, R0", 0).Ok()).IsErr());  // 0b4 not valid binary
        CHECK(ValidateInstruction(ParseLine("MOV $+0B4, R7", 0).Ok()).IsErr());  // 0b4 not valid binary
        CHECK(ValidateInstruction(ParseLine("MOV $+0bFF, R7", 0).Ok()).IsErr()); // 0bFF not valid binary
        CHECK(ValidateInstruction(ParseLine("MOV $-0b01, R7", 0).Ok()).IsErr()); // negative binary not allowed

        // Completely malformed operands
        CHECK(ValidateInstruction(ParseLine("MOV R1, $", 0).Ok()).IsErr());      // empty immediate value
        CHECK(ValidateInstruction(ParseLine("MOV $, $", 0).Ok()).IsErr());       // both sides empty
        CHECK(ValidateInstruction(ParseLine("MOV $-0xZZ, R1", 0).Ok()).IsErr()); // malformed signed hex

        // Source must be valid (either immediate or register)
        CHECK(ValidateInstruction(ParseLine("ADD 4, R1", 0).Ok()).IsErr());   // missing $

        // Both operands missing
        CHECK(ValidateInstruction(ParseLine("MOV", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("ADD", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("SUB", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("CMP", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("LOAD", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("STORE", 0).Ok()).IsErr());

        // Only one operand
        CHECK(ValidateInstruction(ParseLine("MOV R1", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("ADD R1", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("SUB R1", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("CMP R1", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("LOAD R1", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("STORE R1", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("MOV $1", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("ADD $1", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("SUB $1", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("CMP $1", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("LOAD $1", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("STORE $1", 0).Ok()).IsErr());

        // Invalid opcode
        CHECK(ValidateInstruction(ParseLine("XYZ R1, R2", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("XYZ R1", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("XYZ", 0).Ok()).IsErr());
        
        // Load op1 has to be an address or a register and op2 has to be a register
        CHECK(ValidateInstruction(ParseLine("LOAD R1, $3", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("LOAD R1, $0x20", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("LOAD R1, $0b11", 0).Ok()).IsErr());

        // Store op1 has to be a register and op2 has to be an address or a register
        CHECK(ValidateInstruction(ParseLine("STORE  $0x4, R0", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("STORE $+0x4, R7", 0).Ok()).IsErr());
        CHECK(ValidateInstruction(ParseLine("STORE $-0x4, R7", 0).Ok()).IsErr());
    }


    SUBCASE("Valid Instruction")
    {
        CHECK(ValidateInstruction(ParseLine("BRK",  0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("HALT", 0).Ok()).IsOk());
        
        CHECK(ValidateInstruction(ParseLine("JMP Label", 0).Ok(), { "123", "Label", "Test" }).IsOk());
        CHECK(ValidateInstruction(ParseLine("JE  Label", 0).Ok(), { "123", "Label", "Test" }).IsOk());
        CHECK(ValidateInstruction(ParseLine("JMP R1", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("JE  R1", 0).Ok()).IsOk());
        
        CHECK(ValidateInstruction(ParseLine("MOV  $0x4, R0", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("MOV $+0X4, R7", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("MOV $-0x4, R7", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("MOV R1, R0", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("MOV R1, R7", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("MOV R1, R7", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("MOV  $0b101, R0", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("MOV $+0B101, R7", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("MOV  $999, R0", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("MOV $+999, R7", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("MOV $-999, R7", 0).Ok()).IsOk());
        
        CHECK(ValidateInstruction(ParseLine("ADD  $0x4, R0", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("ADD $+0x4, R7", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("ADD $-0x4, R7", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("ADD R1, R0", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("ADD R1, R7", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("ADD R1, R7", 0).Ok()).IsOk());
        
        CHECK(ValidateInstruction(ParseLine("SUB  $0x4, R0", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("SUB $+0x4, R7", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("SUB $-0x4, R7", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("SUB R1, R0", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("SUB R1, R7", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("SUB R1, R7", 0).Ok()).IsOk());
        
        CHECK(ValidateInstruction(ParseLine("CMP  $0x4, R0", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("CMP $+0x4, R7", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("CMP $-0x4, R7", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("CMP R1, R0", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("CMP R1, R7", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("CMP R1, R7", 0).Ok()).IsOk());
        
        CHECK(ValidateInstruction(ParseLine("LOAD  $0x4, R0", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("LOAD $+0x4, R7", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("LOAD $-0x4, R7", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("LOAD R1, R7", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("LOAD R1, R7", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("LOAD R1, R0", 0).Ok()).IsOk());
        
        CHECK(ValidateInstruction(ParseLine("STORE R0, $0xFF", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("STORE R7, $0x29", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("STORE R7, $0b11", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("STORE R1, R0", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("STORE R1, R7", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("STORE R1, R7", 0).Ok()).IsOk());

        CHECK(ValidateInstruction(ParseLine("LOADB  $0x4, R0", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("LOADB $+0x4, R7", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("LOADB $-0x4, R7", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("LOADB R1, R7", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("LOADB R1, R7", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("LOADB R1, R0", 0).Ok()).IsOk());

        CHECK(ValidateInstruction(ParseLine("STOREB R0, $0xFF", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("STOREB R7, $0x29", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("STOREB R7, $0b11", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("STOREB R1, R0", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("STOREB R1, R7", 0).Ok()).IsOk());
        CHECK(ValidateInstruction(ParseLine("STOREB R1, R7", 0).Ok()).IsOk());
    }
}


Result<void, ASMError> ValidateOperand(OperandType, const std::string&, const ParsedInstruction&, std::string_view, const std::unordered_set<std::string>&);
TEST_CASE("Test ValidateOperand()")
{
    SUBCASE("Invalid")
    {
        CHECK(ValidateOperand(OperandType::None,                   "R1", {}, "", {}).IsErr());
        CHECK(ValidateOperand(OperandType::Register,               "$0x04", {}, "", {}).IsErr());
        CHECK(ValidateOperand(OperandType::Register,               "RR", {}, "", {}).IsErr());
        CHECK(ValidateOperand(OperandType::Intermediate,           "R1", {}, "", {}).IsErr());
        CHECK(ValidateOperand(OperandType::Intermediate,           "0xFF", {}, "", {}).IsErr());
        CHECK(ValidateOperand(OperandType::Intermediate,           "$0bFF", {}, "", {}).IsErr());
        CHECK(ValidateOperand(OperandType::RegisterOrIntermediate, "Test", {}, "", {}).IsErr());
        CHECK(ValidateOperand(OperandType::RegisterOrLabel,        "Multiply", {}, "", {}).IsErr());
    }

    SUBCASE("Valid")
    {
        CHECK(ValidateOperand(OperandType::None, "", {}, "", {}).IsOk());
        CHECK(ValidateOperand(OperandType::Register, "R1", {}, "", {}).IsOk());
        CHECK(ValidateOperand(OperandType::Register, "r7", {}, "", {}).IsOk());
        CHECK(ValidateOperand(OperandType::Intermediate, "$0xFF", {}, "", {}).IsOk());
        CHECK(ValidateOperand(OperandType::Intermediate, "$0XFF", {}, "", {}).IsOk());
        CHECK(ValidateOperand(OperandType::Intermediate, "$0b01", {}, "", {}).IsOk());
        CHECK(ValidateOperand(OperandType::Intermediate, "$0B01", {}, "", {}).IsOk());
        CHECK(ValidateOperand(OperandType::Intermediate, "$000212", {}, "", {}).IsOk());
        CHECK(ValidateOperand(OperandType::RegisterOrIntermediate, "$0xFF", {}, "", {}).IsOk());
        CHECK(ValidateOperand(OperandType::RegisterOrIntermediate, "$0XFF", {}, "", {}).IsOk());
        CHECK(ValidateOperand(OperandType::RegisterOrIntermediate, "$0b01", {}, "", {}).IsOk());
        CHECK(ValidateOperand(OperandType::RegisterOrIntermediate, "R1", {}, "", {}).IsOk());
        CHECK(ValidateOperand(OperandType::RegisterOrIntermediate, "R0", {}, "", {}).IsOk());
        CHECK(ValidateOperand(OperandType::RegisterOrIntermediate, "R7", {}, "", {}).IsOk());
        CHECK(ValidateOperand(OperandType::RegisterOrLabel, "R7", {}, "", {}).IsOk());
        CHECK(ValidateOperand(OperandType::RegisterOrLabel, "Hello", {}, "", { "Hello" }).IsOk());
        CHECK(ValidateOperand(OperandType::RegisterOrLabel, "Multiply", {}, "", { "Multiply" }).IsOk());
    }
}


TEST_CASE("Test LookupOpcode()")
{
    SUBCASE("Invalid")
    {
        CHECK_FALSE(LookupOpcode("move").has_value());
        CHECK_FALSE(LookupOpcode("adda").has_value());
        CHECK_FALSE(LookupOpcode("sub1").has_value());
        CHECK_FALSE(LookupOpcode("camp").has_value());
        CHECK_FALSE(LookupOpcode("jfmp").has_value());
        CHECK_FALSE(LookupOpcode("haalt").has_value());
    }

    SUBCASE("Valid")
    {
        CHECK(LookupOpcode("mov").has_value());
        CHECK(LookupOpcode("add").has_value());
        CHECK(LookupOpcode("sub").has_value());
        CHECK(LookupOpcode("cmp").has_value());
        CHECK(LookupOpcode("jmp").has_value());
        CHECK(LookupOpcode("halt").has_value());

        CHECK(LookupOpcode("MOV").has_value());
        CHECK(LookupOpcode("ADD").has_value());
        CHECK(LookupOpcode("SUB").has_value());
        CHECK(LookupOpcode("CMP").has_value());
        CHECK(LookupOpcode("BRK").has_value());
        CHECK(LookupOpcode("HALT").has_value());
        CHECK(LookupOpcode("JMP").has_value());
        CHECK(LookupOpcode("JE").has_value());
        CHECK(LookupOpcode("STORE").has_value());
        CHECK(LookupOpcode("STOREB").has_value());
        CHECK(LookupOpcode("LOAD").has_value());
        CHECK(LookupOpcode("LOADB").has_value());
    }
}


bool IsValidRegister(std::string s);
TEST_CASE("Test IsValidRegister()")
{
    SUBCASE("Invalid")
    {
        CHECK_FALSE(IsValidRegister("R"));
        CHECK_FALSE(IsValidRegister("AA"));
        CHECK_FALSE(IsValidRegister(""));
        CHECK_FALSE(IsValidRegister(" "));
        CHECK_FALSE(IsValidRegister("RR"));
        CHECK_FALSE(IsValidRegister("RT"));
        CHECK_FALSE(IsValidRegister("R-1"));
        CHECK_FALSE(IsValidRegister("RU"));
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
        
        CHECK(IsValidRegister("R8"));
        CHECK(IsValidRegister("R9"));
        CHECK(IsValidRegister("RA"));
        CHECK(IsValidRegister("RB"));
        CHECK(IsValidRegister("RC"));
        CHECK(IsValidRegister("RD"));
        CHECK(IsValidRegister("RE"));
        CHECK(IsValidRegister("RSP"));
        CHECK(IsValidRegister("RBP"));
        
        CHECK(IsValidRegister("r0"));
        CHECK(IsValidRegister("r1"));
        CHECK(IsValidRegister("r2"));
        CHECK(IsValidRegister("r3"));
        CHECK(IsValidRegister("r4"));
        CHECK(IsValidRegister("r5"));
        CHECK(IsValidRegister("r6"));
        CHECK(IsValidRegister("r7"));
        CHECK(IsValidRegister("r8"));
        CHECK(IsValidRegister("r9"));
        CHECK(IsValidRegister("ra"));
        CHECK(IsValidRegister("rb"));
        CHECK(IsValidRegister("rc"));
        CHECK(IsValidRegister("rd"));
        CHECK(IsValidRegister("re"));
        CHECK(IsValidRegister("rsp"));
        CHECK(IsValidRegister("rbp"));
    }
}


bool IsValidIntermediate(std::string_view s);
TEST_CASE("IsValidImmediatie()")
{
    SUBCASE("Invalid")
    {
        // Empty or whitespace
        CHECK_FALSE(IsValidIntermediate(""));
        CHECK_FALSE(IsValidIntermediate(" "));
        CHECK_FALSE(IsValidIntermediate("  "));

        // Not starting with $
        CHECK_FALSE(IsValidIntermediate("R"));
        CHECK_FALSE(IsValidIntermediate("R1"));
        CHECK_FALSE(IsValidIntermediate("1"));
        CHECK_FALSE(IsValidIntermediate("99"));
        CHECK_FALSE(IsValidIntermediate("999"));
        CHECK_FALSE(IsValidIntermediate("0x99"));
        CHECK_FALSE(IsValidIntermediate("0x"));
        CHECK_FALSE(IsValidIntermediate("0x9"));
        CHECK_FALSE(IsValidIntermediate("0b01"));
        CHECK_FALSE(IsValidIntermediate("-0b01"));

        // Malformed dollar-prefixed
        CHECK_FALSE(IsValidIntermediate("$"));             // only $
        CHECK_FALSE(IsValidIntermediate("$ "));            // $ with space
        CHECK_FALSE(IsValidIntermediate("$-"));            // $ minus nothing
        CHECK_FALSE(IsValidIntermediate("$--1"));          // double minus
        CHECK_FALSE(IsValidIntermediate("$++1"));          // double plus
        CHECK_FALSE(IsValidIntermediate("$+-1"));          // invalid mixed signs
        CHECK_FALSE(IsValidIntermediate("$-0b01"));        // binary cannot be signed
        CHECK_FALSE(IsValidIntermediate("$0x"));           // no digits
        CHECK_FALSE(IsValidIntermediate("$0xGHI"));        // invalid hex
        CHECK_FALSE(IsValidIntermediate("$0b"));           // no bits
        CHECK_FALSE(IsValidIntermediate("$0b012"));        // invalid binary
        CHECK_FALSE(IsValidIntermediate("$0b2"));          // binary with '2'
        CHECK_FALSE(IsValidIntermediate("$0b00a"));        // binary with letter
        CHECK_FALSE(IsValidIntermediate("$0x123Z"));       // invalid hex tail
        CHECK_FALSE(IsValidIntermediate("$10A"));          // decimal with letter
        CHECK_FALSE(IsValidIntermediate("$10.1"));         // floats not supported
        CHECK_FALSE(IsValidIntermediate("$+"));            // incomplete
        CHECK_FALSE(IsValidIntermediate("$-"));            // incomplete
        CHECK_FALSE(IsValidIntermediate("$-0x"));          // no hex digits
        CHECK_FALSE(IsValidIntermediate("$+0b"));          // no binary digits
        CHECK_FALSE(IsValidIntermediate("$+0xG"));         // invalid hex digit
        CHECK_FALSE(IsValidIntermediate("$0x+4"));         // misplaced sign
        CHECK_FALSE(IsValidIntermediate("$+00xF"));        // invalid hex digit
        CHECK_FALSE(IsValidIntermediate("$000b11"));       // invalid binary digit

        // Misplaced sign or multiple signs
        CHECK_FALSE(IsValidIntermediate("$++0x4"));
        CHECK_FALSE(IsValidIntermediate("$--0x4"));
        CHECK_FALSE(IsValidIntermediate("$0x-4"));  // sign must come before 0x
    }
    
    
    SUBCASE("Valid")
    {
        // Decimal
        CHECK(IsValidIntermediate("$0"));
        CHECK(IsValidIntermediate("$1"));
        CHECK(IsValidIntermediate("$42"));
        CHECK(IsValidIntermediate("$+123"));
        CHECK(IsValidIntermediate("$-999"));
        CHECK(IsValidIntermediate("$000123"));

        // Hexadecimal
        CHECK(IsValidIntermediate("$0x0"));
        CHECK(IsValidIntermediate("$0x1"));
        CHECK(IsValidIntermediate("$0xFF"));
        CHECK(IsValidIntermediate("$0XdeadBEEF"));
        CHECK(IsValidIntermediate("$+0xABC"));
        CHECK(IsValidIntermediate("$-0x10"));

        // Binary
        CHECK(IsValidIntermediate("$0b0"));
        CHECK(IsValidIntermediate("$0b1"));
        CHECK(IsValidIntermediate("$0b01"));
        CHECK(IsValidIntermediate("$0B101010"));
        CHECK(IsValidIntermediate("$+0b11"));  // '+' allowed even though not meaningful
        CHECK(IsValidIntermediate("$0b00001111"));

        // Edge cases
        CHECK(IsValidIntermediate("$+0"));
        CHECK(IsValidIntermediate("$-0"));
        CHECK(IsValidIntermediate("$+0x0"));
        CHECK(IsValidIntermediate("$-0x0"));

        // Upper and lower case consistency
        CHECK(IsValidIntermediate("$0XFF"));
        CHECK(IsValidIntermediate("$0xFF"));
        CHECK(IsValidIntermediate("$0B10"));
        CHECK(IsValidIntermediate("$0b10"));
    }
}