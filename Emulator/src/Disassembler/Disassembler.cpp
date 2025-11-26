#include <vector>
#include <string>
#include <cstdint>
#include <utility>
#include <unordered_map>

#include "../../Assembler/lib/src/Assembler.hpp"

#include "Log.hpp"
#include "Disassembler.hpp"

std::string Disassembler::DisassembleOperands(OpcodeMC opcode, const std::vector<std::uint8_t>& machineCode, std::uint16_t index) const
{
    switch (opcode)
    {
    case OpcodeMC::LOAD_ADD_TO_REG:
    case OpcodeMC::LOADB_ADD_TO_REG:
    {
        const std::uint16_t immediateValue = static_cast<std::uint16_t>((machineCode.at(index + 2) << 8) | machineCode.at(index + 1));
        return std::format(" $0x{:04X}, {}", immediateValue, m_RegisterToStringMap.at(machineCode.at(index + 3)));
    }
    case OpcodeMC::MOV_IMM_TO_REG:
    case OpcodeMC::ADD_IMM_TO_REG:
    case OpcodeMC::SUB_IMM_TO_REG:
    case OpcodeMC::MUL_IMM_TO_REG:
    case OpcodeMC::IMUL_IMM_TO_REG:
    case OpcodeMC::DIV_IMM_TO_REG:
    case OpcodeMC::IDIV_IMM_TO_REG:
    case OpcodeMC::CMP_IMM_TO_REG:
    case OpcodeMC::AND_IMM_TO_REG:
    case OpcodeMC::XOR_IMM_TO_REG:
    case OpcodeMC::OR_IMM_TO_REG:
    {
        const std::uint16_t immediateValue = static_cast<std::uint16_t>((machineCode.at(index + 2) << 8) | machineCode.at(index + 1));
        return std::format(" $0x{:X}, {}", immediateValue, m_RegisterToStringMap.at(machineCode.at(index + 3)));
    }
    case OpcodeMC::MOV_REG_TO_REG:
    case OpcodeMC::ADD_REG_TO_REG:
    case OpcodeMC::SUB_REG_TO_REG:
    case OpcodeMC::MUL_REG_TO_REG:
    case OpcodeMC::IMUL_REG_TO_REG:
    case OpcodeMC::DIV_REG_TO_REG:
    case OpcodeMC::IDIV_REG_TO_REG:
    case OpcodeMC::CMP_REG_TO_REG:
    case OpcodeMC::EXTBH_REG_TO_REG:
    case OpcodeMC::EXTBL_REG_TO_REG:
    case OpcodeMC::INSBH_REG_TO_REG:
    case OpcodeMC::AND_REG_TO_REG:
    case OpcodeMC::OR_REG_TO_REG:
    case OpcodeMC::XOR_REG_TO_REG:
    case OpcodeMC::INSBL_REG_TO_REG:
    case OpcodeMC::LOAD_REG_TO_REG:
    case OpcodeMC::LOADB_REG_TO_REG:
    case OpcodeMC::STORE_REG_TO_REG:
    case OpcodeMC::STOREB_REG_TO_REG:
        return std::format(" {}, {}", m_RegisterToStringMap.at(machineCode.at(index + 1)), m_RegisterToStringMap.at(machineCode.at(index + 2)));
    case OpcodeMC::JE_REG:
    case OpcodeMC::JMP_REG:
    case OpcodeMC::SWAPB_REG:
    case OpcodeMC::NEG_REG:
        return std::format(" {}", m_RegisterToStringMap.at(machineCode.at(index + 1)));
    case OpcodeMC::JE_LABEL:
    case OpcodeMC::JMP_LABEL:
    {
        const std::uint16_t immediateValue = static_cast<std::uint16_t>((machineCode.at(index + 2) << 8) | machineCode.at(index + 1));
        return std::format(" $0x{:04X}", immediateValue);
    }
    case OpcodeMC::STORE_REG_TO_ADD:
    case OpcodeMC::STOREB_REG_TO_ADD:
    {
        const std::uint16_t immediateValue = static_cast<std::uint16_t>((machineCode.at(index + 3) << 8) | machineCode.at(index + 2));
        return std::format(" {}, $0x{:04X}", m_RegisterToStringMap.at(machineCode.at(index + 1)), immediateValue);
    }
    case OpcodeMC::NOP:
    case OpcodeMC::BRK:
    case OpcodeMC::HALT:
        return std::string();
    }
    return " ???, ???";
}


void Disassembler::Disassemble(const std::vector<std::uint8_t>& machineCode)
{
    m_SourceCode.clear();
    m_SourceCode.reserve(machineCode.size());

    for (std::uint16_t i = 0; i < machineCode.size();)
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
            m_ErrorMsg = std::format("Failed to disassemble machine code, instruction: {} | 0x{:X}, at address: {} | 0x{:04X}", (int)machineCode[i], (int)machineCode[i], i, i);
            return;
        }
    }
    m_ErrorMsg.clear();
}