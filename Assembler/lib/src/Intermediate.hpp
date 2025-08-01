#ifndef INTERMEDIATE_HPP
#define INTERMEDIATE_HPP
#include <string>
#include <cstdint>
#include <variant>

#include "Parser.hpp"

enum class OpcodeIR
{
    MOV_IMM_TO_REG,
    ADD_IMM_TO_REG,
    SUB_IMM_TO_REG,
    CMP_IMM_TO_REG,

    MOV_REG_TO_REG,
    ADD_REG_TO_REG,
    SUB_REG_TO_REG,
    CMP_REG_TO_REG,

    JMP_REG,
    JMP_LABEL,
    JE_REG,
    JE_LABEL,

    HLT,
    LOAD,
    STORE
};


enum class OperandTypeIR
{
    Register,
    Intermediate,
    Label,
    None
};

struct OperandIR
{
    OperandTypeIR type;
    std::variant<std::uint8_t, std::uint16_t, std::string> value;
};

struct InstructionIR
{
    std::string label;
    OpcodeIR opcode;
    OperandIR op1;
    OperandIR op2;
};

InstructionIR LowerInstruction(const ParsedInstruction& parsedInstr);

#endif // INTERMEDIATE_HPP