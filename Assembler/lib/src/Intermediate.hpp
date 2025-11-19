#ifndef INTERMEDIATE_HPP
#define INTERMEDIATE_HPP
#include <string>
#include <cstdint>
#include <variant>
#include <unordered_map>

#include "Parser.hpp"

// Opcode values from SPEC.txt
enum class OpcodeIR
{
    MOV_IMM_TO_REG = 20,
    ADD_IMM_TO_REG = 30,
    SUB_IMM_TO_REG = 32,
    CMP_IMM_TO_REG = 60,

    MOV_REG_TO_REG = 21,
    ADD_REG_TO_REG = 31,
    SUB_REG_TO_REG = 33,
    CMP_REG_TO_REG = 61,

    JMP_REG = 50,
    JMP_LABEL = 51,
    JE_REG = 52,
    JE_LABEL = 53,

    HLT = 0xFF,
    LOAD_ADD_TO_REG = 10,
    LOAD_REG_TO_REG = 11,
    STORE_REG_TO_ADD = 12,
    STORE_REG_TO_REG = 13
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
    std::uint32_t size; // Instruction size in bytes
};

static const std::unordered_map<OpcodeIR, std::uint32_t> s_InstructionIRSizeMap = {
    { OpcodeIR::MOV_IMM_TO_REG  , 4 },
    { OpcodeIR::ADD_IMM_TO_REG  , 4 },
    { OpcodeIR::SUB_IMM_TO_REG  , 4 },
    { OpcodeIR::CMP_IMM_TO_REG  , 4 },
    { OpcodeIR::MOV_REG_TO_REG  , 3 },
    { OpcodeIR::ADD_REG_TO_REG  , 3 },
    { OpcodeIR::SUB_REG_TO_REG  , 3 },
    { OpcodeIR::CMP_REG_TO_REG  , 3 },
    { OpcodeIR::JMP_REG         , 2 },
    { OpcodeIR::JMP_LABEL       , 3 },
    { OpcodeIR::JE_REG          , 2 },
    { OpcodeIR::JE_LABEL        , 3 },
    { OpcodeIR::HLT             , 1 },
    { OpcodeIR::LOAD_ADD_TO_REG , 4 },
    { OpcodeIR::LOAD_REG_TO_REG , 3 },
    { OpcodeIR::STORE_REG_TO_ADD, 4 },
    { OpcodeIR::STORE_REG_TO_REG, 3 },
};

InstructionIR LowerInstruction(const ParsedInstruction& parsedInstr);

#endif // INTERMEDIATE_HPP