#ifndef INTERMEDIATE_HPP
#define INTERMEDIATE_HPP
#include <string>
#include <cstdint>

#include "Parser.hpp"

enum class OpcodeIR
{
    MOV,
    ADD,
    SUB,
    CMP,
    JMP,
    JE,
    HLT,
    LOAD,
    STORE
};


enum class OperandType
{
    Register,
    Immediate
};

struct Operand
{
    OperandType type;
    std::uint16_t value;
};

struct InstructionIR
{
    std::string label;
    OpcodeIR opcode;
    Operand op1;
    Operand op2;
};

InstructionIR LowerInstruction(const ParsedInstruction& parsedInstr);

#endif // INTERMEDIATE_HPP