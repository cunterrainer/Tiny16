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
    NOP = 0,
    HALT = 1,

    MOV_IMM_TO_REG  = 20,
    ADD_IMM_TO_REG  = 30,
    MUL_IMM_TO_REG  = 31,
    IMUL_IMM_TO_REG = 32,
    DIV_IMM_TO_REG  = 33,
    IDIV_IMM_TO_REG = 34,
    SUB_IMM_TO_REG  = 35,
    CMP_IMM_TO_REG  = 60,

    MOV_REG_TO_REG  = 21,
    ADD_REG_TO_REG  = 36,
    SUB_REG_TO_REG  = 37,
    MUL_REG_TO_REG  = 38,
    IMUL_REG_TO_REG = 39,
    DIV_REG_TO_REG  = 40,
    IDIV_REG_TO_REG = 41,
    CMP_REG_TO_REG  = 61,

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

    EXTBH_REG_TO_REG = 100,
    EXTBL_REG_TO_REG = 101,
    INSBH_REG_TO_REG = 102,
    INSBL_REG_TO_REG = 103,
    SWAPB_REG        = 104,
    AND_REG_TO_REG   = 110,
    AND_IMM_TO_REG   = 111,
    OR_REG_TO_REG    = 112,
    OR_IMM_TO_REG    = 113,
    XOR_REG_TO_REG   = 114,
    XOR_IMM_TO_REG   = 115,
    NEG_REG          = 116,

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
    { OpcodeIR::MUL_IMM_TO_REG   , 4 },
    { OpcodeIR::IMUL_IMM_TO_REG  , 4 },
    { OpcodeIR::DIV_IMM_TO_REG   , 4 },
    { OpcodeIR::IDIV_IMM_TO_REG  , 4 },
    { OpcodeIR::CMP_IMM_TO_REG   , 4 },
    { OpcodeIR::MOV_REG_TO_REG   , 3 },
    { OpcodeIR::ADD_REG_TO_REG   , 3 },
    { OpcodeIR::SUB_REG_TO_REG   , 3 },
    { OpcodeIR::MUL_REG_TO_REG   , 3 },
    { OpcodeIR::IMUL_REG_TO_REG  , 3 },
    { OpcodeIR::DIV_REG_TO_REG   , 3 },
    { OpcodeIR::IDIV_REG_TO_REG  , 3 },
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
    { OpcodeIR::EXTBH_REG_TO_REG , 3 },
    { OpcodeIR::EXTBL_REG_TO_REG , 3 },
    { OpcodeIR::INSBH_REG_TO_REG , 3 },
    { OpcodeIR::INSBL_REG_TO_REG , 3 },
    { OpcodeIR::SWAPB_REG        , 2 },
    { OpcodeIR::AND_REG_TO_REG   , 3 },
    { OpcodeIR::AND_IMM_TO_REG   , 4 },
    { OpcodeIR::OR_REG_TO_REG    , 3 },
    { OpcodeIR::OR_IMM_TO_REG    , 4 },
    { OpcodeIR::XOR_REG_TO_REG   , 3 },
    { OpcodeIR::XOR_IMM_TO_REG   , 4 },
    { OpcodeIR::NEG_REG          , 2 },
    { OpcodeIR::NOP              , 1 }
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
    { Opcode::MUL,    {1, OpcodeIR::MUL_REG_TO_REG,     OpcodeIR::MUL_IMM_TO_REG  }},
    { Opcode::IMUL,   {1, OpcodeIR::IMUL_REG_TO_REG,    OpcodeIR::IMUL_IMM_TO_REG }},
    { Opcode::DIV,    {1, OpcodeIR::DIV_REG_TO_REG,     OpcodeIR::DIV_IMM_TO_REG  }},
    { Opcode::IDIV,   {1, OpcodeIR::IDIV_REG_TO_REG,    OpcodeIR::IDIV_IMM_TO_REG }},
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
    { Opcode::NOP,    {0, OpcodeIR::NOP,                OpcodeIR::NOP             }},

    // Logical Operations
    {Opcode::AND,    {1, OpcodeIR::AND_REG_TO_REG,     OpcodeIR::AND_IMM_TO_REG}},
    {Opcode::OR,     {1, OpcodeIR::OR_REG_TO_REG,      OpcodeIR::OR_IMM_TO_REG}},
    {Opcode::XOR,    {1, OpcodeIR::XOR_REG_TO_REG,     OpcodeIR::XOR_IMM_TO_REG}},

    // Unary / Single Operand
    {Opcode::NEG,    {0, OpcodeIR::NEG_REG,            OpcodeIR::NEG_REG}},
    {Opcode::SWAPB,  {0, OpcodeIR::SWAPB_REG,          OpcodeIR::SWAPB_REG}},

    // Byte Manipulation
    {Opcode::EXTBH,  {0, OpcodeIR::EXTBH_REG_TO_REG,   OpcodeIR::EXTBH_REG_TO_REG}},
    {Opcode::EXTBL,  {0, OpcodeIR::EXTBL_REG_TO_REG,   OpcodeIR::EXTBL_REG_TO_REG}},
    {Opcode::INSBH,  {0, OpcodeIR::INSBH_REG_TO_REG,   OpcodeIR::INSBH_REG_TO_REG}},
    {Opcode::INSBL,  {0, OpcodeIR::INSBL_REG_TO_REG,   OpcodeIR::INSBL_REG_TO_REG}}
};

InstructionIR LowerInstruction(const ParsedInstruction& parsedInstr);
std::vector<InstructionIR> LowerAllInstructions(const std::vector<ParsedInstruction>& parsedInstructions);

#endif // INTERMEDIATE_HPP