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
    JE,
    EXTBH,
    EXTBL,
    INSBH,
    INSBL,
    SWAPB,
    AND,
    OR,
    XOR,
    NEG,
    NOP
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
    { "JE",     { Opcode::JE,     OperandType::RegisterOrLabel,        OperandType::None } },

    { "EXTBH",  { Opcode::EXTBH,   OperandType::Register,        OperandType::Register } },
    { "EXTBL",  { Opcode::EXTBL,   OperandType::Register,        OperandType::Register } },
    { "INSBH",  { Opcode::INSBH,   OperandType::Register,        OperandType::Register } },
    { "INSBL",  { Opcode::INSBL,   OperandType::Register,        OperandType::Register } },
    { "SWAPB",  { Opcode::SWAPB,   OperandType::Register,        OperandType::None } },

    { "AND",    { Opcode::AND,     OperandType::RegisterOrIntermediate,  OperandType::Register } },
    { "OR",     { Opcode::OR,      OperandType::RegisterOrIntermediate,  OperandType::Register } },
    { "XOR",    { Opcode::XOR,     OperandType::RegisterOrIntermediate,  OperandType::Register } },
    { "NEG",    { Opcode::NEG,     OperandType::Register,                OperandType::None     } },
    { "NOP",    { Opcode::NOP,     OperandType::None,                    OperandType::None     } }
};

#endif // INSTRUCTION_H