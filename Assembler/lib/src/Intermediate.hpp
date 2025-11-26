#ifndef INTERMEDIATE_HPP
#define INTERMEDIATE_HPP
#include <string>
#include <cstdint>
#include <variant>
#include <unordered_map>

#include "Parser.hpp"
#include "Instruction.hpp"

// Opcode values from SPEC.txt
enum class OpcodeIR
{
    HALT = 1,

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

    LOAD_ADD_TO_REG = 10,
    LOAD_REG_TO_REG = 11,
    LOADB_ADD_TO_REG = 12,
    LOADB_REG_TO_REG = 13,
    STORE_REG_TO_ADD = 14,
    STORE_REG_TO_REG = 15,
    STOREB_REG_TO_ADD = 16,
    STOREB_REG_TO_REG = 17,

    BRK = 0xFF
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
    std::uint16_t size; // Instruction size in bytes
};

static const std::unordered_map<OpcodeIR, std::uint16_t> s_InstructionIRSizeMap = {
    { OpcodeIR::MOV_IMM_TO_REG   , 4 },
    { OpcodeIR::ADD_IMM_TO_REG   , 4 },
    { OpcodeIR::SUB_IMM_TO_REG   , 4 },
    { OpcodeIR::CMP_IMM_TO_REG   , 4 },
    { OpcodeIR::MOV_REG_TO_REG   , 3 },
    { OpcodeIR::ADD_REG_TO_REG   , 3 },
    { OpcodeIR::SUB_REG_TO_REG   , 3 },
    { OpcodeIR::CMP_REG_TO_REG   , 3 },
    { OpcodeIR::JMP_REG          , 2 },
    { OpcodeIR::JMP_LABEL        , 3 },
    { OpcodeIR::JE_REG           , 2 },
    { OpcodeIR::JE_LABEL         , 3 },
    { OpcodeIR::HALT             , 1 },
    { OpcodeIR::BRK              , 1 },
    { OpcodeIR::LOAD_ADD_TO_REG  , 4 },
    { OpcodeIR::LOAD_REG_TO_REG  , 3 },
    { OpcodeIR::LOADB_ADD_TO_REG , 4 },
    { OpcodeIR::LOADB_REG_TO_REG , 3 },
    { OpcodeIR::STORE_REG_TO_ADD , 4 },
    { OpcodeIR::STORE_REG_TO_REG , 3 },
    { OpcodeIR::STOREB_REG_TO_ADD, 4 },
    { OpcodeIR::STOREB_REG_TO_REG, 3 },
};


struct OpcodeIRMapping
{
    int relevantOperandIndex; // Which operand controls the logic? (0 = None, 1 = op1, 2 = op2)
    OpcodeIR resIfRegister; // Result if the relevant operand is a Register
    OpcodeIR resIfOther; // Result if the relevant operand is Immediate/Label/Address
};


static const std::unordered_map<Opcode, OpcodeIRMapping> s_OpcodeIRMapping = {
    // Opcode    | Check Op? | If Register returns...  | If Other returns...
    { Opcode::MOV,    {1, OpcodeIR::MOV_REG_TO_REG,     OpcodeIR::MOV_IMM_TO_REG  }},
    { Opcode::ADD,    {1, OpcodeIR::ADD_REG_TO_REG,     OpcodeIR::ADD_IMM_TO_REG  }},
    { Opcode::SUB,    {1, OpcodeIR::SUB_REG_TO_REG,     OpcodeIR::SUB_IMM_TO_REG  }},
    { Opcode::CMP,    {1, OpcodeIR::CMP_REG_TO_REG,     OpcodeIR::CMP_IMM_TO_REG  }},
    { Opcode::LOAD,   {1, OpcodeIR::LOAD_REG_TO_REG,    OpcodeIR::LOAD_ADD_TO_REG }},
    { Opcode::LOADB,  {1, OpcodeIR::LOADB_REG_TO_REG,   OpcodeIR::LOADB_ADD_TO_REG }},
    { Opcode::JMP,    {1, OpcodeIR::JMP_REG,            OpcodeIR::JMP_LABEL       }},
    { Opcode::JE,     {1, OpcodeIR::JE_REG,             OpcodeIR::JE_LABEL        }},

    // STORE checks Op2, not Op1
    { Opcode::STORE,  {2, OpcodeIR::STORE_REG_TO_REG,   OpcodeIR::STORE_REG_TO_ADD}},
    { Opcode::STOREB, {2, OpcodeIR::STOREB_REG_TO_REG,  OpcodeIR::STOREB_REG_TO_ADD}},

    // Single byte instructions (Don't care about operands)
    { Opcode::HALT,   {0, OpcodeIR::HALT,               OpcodeIR::HALT            }},
    { Opcode::BRK,    {0, OpcodeIR::BRK,                OpcodeIR::BRK             }},
};

InstructionIR LowerInstruction(const ParsedInstruction& parsedInstr);
std::vector<InstructionIR> LowerAllInstructions(const std::vector<ParsedInstruction>& parsedInstructions);

#endif // INTERMEDIATE_HPP