#ifndef MACHINE_CODE_GENERATOR_HPP
#define MACHINE_CODE_GENERATOR_HPP

#include <vector>
#include <cstdint>

#include "Assembler.hpp"

std::vector<std::uint8_t> GenerateMachineCode(const std::vector<InstructionMC>& assembledInstructions)
{
    std::vector<std::uint8_t> machineCode;
    machineCode.reserve(assembledInstructions.size() * 4); // 4 is max size for our instructions atm

    for (const auto& instr : assembledInstructions)
    {
        machineCode.push_back(instr.opcode);
        if (instr.op1.type == OperandTypeMC::Register)
        {
            machineCode.push_back(std::get<std::uint8_t>(instr.op1.value));
        }
        else if (instr.op1.type == OperandTypeMC::Intermediate)
        {
            // writes it in little endian order
            const std::uint16_t v = std::get<std::uint16_t>(instr.op1.value);
            machineCode.push_back(static_cast<std::uint8_t>(v & 0xFF));
            machineCode.push_back(static_cast<std::uint8_t>((v >> 8) & 0xFF));
        }

        if (instr.op2.type == OperandTypeMC::Register)
        {
            machineCode.push_back(std::get<std::uint8_t>(instr.op2.value));
        }
        else if (instr.op2.type == OperandTypeMC::Intermediate)
        {
            // writes it in little endian order
            const std::uint16_t v = std::get<std::uint16_t>(instr.op2.value);
            machineCode.push_back(static_cast<std::uint8_t>(v & 0xFF));
            machineCode.push_back(static_cast<std::uint8_t>((v >> 8) & 0xFF));
        }
    }

    return machineCode;
}

#endif // MACHINE_CODE_GENERATOR_HPP