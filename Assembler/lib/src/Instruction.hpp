#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <cstdint>
#include <variant>
#include <string_view>
#include <unordered_map>

enum class Opcode
{
    MOV,
    ADD,
    SUB,
    CMP,
    LOAD,
    LOADB,
    STORE,
    STOREB,
    HALT,
    BRK,
    JMP,
    JE
};


enum class OperandType
{
    None,
    Label,
    Register,
    Intermediate,
    RegisterOrIntermediate,
    RegisterOrLabel
};


struct Instruction
{
    Opcode opcode;
    OperandType op1;
    OperandType op2;
};

static const std::unordered_map<std::string_view, Instruction> s_InstructionMap = {
    { "MOV",    { Opcode::MOV, OperandType::RegisterOrIntermediate, OperandType::Register } },
    { "ADD",    { Opcode::ADD, OperandType::RegisterOrIntermediate, OperandType::Register } },
    { "SUB",    { Opcode::SUB, OperandType::RegisterOrIntermediate, OperandType::Register } },
    { "CMP",    { Opcode::CMP, OperandType::RegisterOrIntermediate, OperandType::Register } },

    { "LOAD",   { Opcode::LOAD,   OperandType::RegisterOrIntermediate, OperandType::Register } },
    { "LOADB",  { Opcode::LOADB,  OperandType::RegisterOrIntermediate, OperandType::Register } },
    { "STORE",  { Opcode::STORE,  OperandType::Register,               OperandType::RegisterOrIntermediate } },
    { "STOREB", { Opcode::STOREB, OperandType::Register,               OperandType::RegisterOrIntermediate } },
    { "HALT",   { Opcode::HALT,   OperandType::None,                   OperandType::None } },
    { "BRK",    { Opcode::BRK,    OperandType::None,                   OperandType::None } },
    { "JMP",    { Opcode::JMP,    OperandType::RegisterOrLabel,        OperandType::None } },
    { "JE",     { Opcode::JE,     OperandType::RegisterOrLabel,        OperandType::None } }
};

#endif // INSTRUCTION_H