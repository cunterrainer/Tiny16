#include <vector>
#include <string>
#include <cstdint>
#include <utility>
#include <unordered_map>

#include "../../Assembler/lib/src/Assembler.hpp"

#include "Log.hpp"
#include "Disassembler.hpp"

std::string Disassembler::DisassembleOperands(OpcodeMC opcode, const std::vector<std::uint8_t>& machineCode, size_t index) const
{
    switch (opcode)
    {
    case OpcodeMC::LOAD:
    {
        const std::uint16_t immediateValue = (machineCode.at(index + 2) << 8) | machineCode.at(index + 1);
        return std::format(" $0x{:04X}, R{}", immediateValue, machineCode.at(index + 3));
    }
    case OpcodeMC::MOV_IMM_TO_REG:
    case OpcodeMC::ADD_IMM_TO_REG:
    case OpcodeMC::SUB_IMM_TO_REG:
    case OpcodeMC::CMP_IMM_TO_REG:
    {
        const std::uint16_t immediateValue = (machineCode.at(index + 2) << 8) | machineCode.at(index + 1);
        return std::format(" $0x{:X}, R{}", immediateValue, machineCode.at(index + 3));
    }
    case OpcodeMC::MOV_REG_TO_REG:
    case OpcodeMC::ADD_REG_TO_REG:
    case OpcodeMC::SUB_REG_TO_REG:
    case OpcodeMC::CMP_REG_TO_REG:
        return std::format(" R{}, R{}", machineCode.at(index + 1), machineCode.at(index + 2));
    case OpcodeMC::JE_REG:
    case OpcodeMC::JMP_REG:
        return std::format(" R{}", machineCode.at(index + 1));
    case OpcodeMC::JE_LABEL:
    case OpcodeMC::JMP_LABEL:
    {
        const std::uint16_t immediateValue = (machineCode.at(index + 2) << 8) | machineCode.at(index + 1);
        return std::format(" $0x{:04X}", immediateValue);
    }
    case OpcodeMC::STORE:
    {
        const std::uint16_t immediateValue = (machineCode.at(index + 3) << 8) | machineCode.at(index + 2);
        return std::format(" R{}, $0x{:04X}", machineCode.at(index + 1), immediateValue);
    }
    case OpcodeMC::HLT:
        return std::string();
    }
    return " ???, ???";
}


Disassembler::Disassembler(const std::vector<std::uint8_t>& machineCode)
{
    m_SourceCode.reserve(machineCode.size());

    for (size_t i = 0; i < machineCode.size();)
    {
        try
        {
            const OpcodeMC opcode = static_cast<OpcodeMC>(machineCode[i]);
            const std::string opcodeStr(m_OpcodeToStringMap.at(opcode));
            const std::string operands = DisassembleOperands(opcode, machineCode, i);

            m_SourceCode.emplace_back(i, std::format("0x{:04X}: {}", i, opcodeStr + operands));
            i += s_InstructionIRSizeMap.at(opcode);
        }
        catch (const std::out_of_range&)
        {
            ERR("Failed to disassemble source code, instruction: {} | 0x{:X}, at address: {} | 0x{:04X}", (int)machineCode[i], (int)machineCode[i], i, i);
        }
    }
}