#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <vector>
#include <limits>
#include <cstdint>
#include <variant>

#include "Intermediate.hpp"

#include "Utility/Result.hpp"

using OpcodeMC = OpcodeIR;

enum class OperandTypeMC
{
    None,
    Register,
    Intermediate
};

struct OperandMC
{
    OperandTypeMC type;
    std::variant<std::uint8_t, std::uint16_t> value;
};


struct InstructionMC
{
    std::uint8_t opcode;
    OperandMC op1;
    OperandMC op2;
};


InstructionMC AssembleInstruction(const InstructionIR& instrIr, const std::unordered_map<std::string, std::uint32_t>& labelAddress);
std::uint32_t ResolveLabelAddress(std::string_view label, const std::vector<InstructionIR>& instr, std::uint32_t offset);
Result<std::vector<InstructionMC>> AssembleInstructions(const std::vector<InstructionIR>& instr);

#endif // ASSEMBLER_H