#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <vector>
#include <cstdint>
#include <stdexcept>

#include "../../Assembler/lib/src/Assembler.hpp"

#include "Log.hpp"

class Disassembler
{
private:
    std::vector<std::string> m_SourceCode;

    const std::unordered_map<OpcodeMC, std::string_view> m_OpcodeToStringMap = {
        { OpcodeIR::MOV_IMM_TO_REG, "MOV" },
        { OpcodeIR::ADD_IMM_TO_REG, "ADD" },
        { OpcodeIR::SUB_IMM_TO_REG, "SUB" },
        { OpcodeIR::CMP_IMM_TO_REG, "CMP" },
        { OpcodeIR::MOV_REG_TO_REG, "MOV" },
        { OpcodeIR::ADD_REG_TO_REG, "ADD" },
        { OpcodeIR::SUB_REG_TO_REG, "SUB" },
        { OpcodeIR::CMP_REG_TO_REG, "CMP" },
        { OpcodeIR::JMP_REG       , "JMP" },
        { OpcodeIR::JMP_LABEL     , "JMP" },
        { OpcodeIR::JE_REG        , "JE" },
        { OpcodeIR::JE_LABEL      , "JE" },
        { OpcodeIR::HLT           , "HTL" },
        { OpcodeIR::LOAD          , "LOAD" },
        { OpcodeIR::STORE         , "STORE" },
    };
public:
    explicit Disassembler(const std::vector<std::uint8_t>& machineCode)
    {
        m_SourceCode.reserve(machineCode.size());

        for (size_t i = 0; i < machineCode.size();)
        {
            try
            {
                const OpcodeMC opcode = static_cast<OpcodeMC>(machineCode[i]);
                m_SourceCode.emplace_back(m_OpcodeToStringMap.at(opcode));
                i += s_InstructionIRSizeMap.at(opcode);
            }
            catch (const std::out_of_range& e)
            {
                ERR("Failed to disassemble source code, instruction: {} | 0x{:X}, at address: {} | 0x{:04X}", (int)machineCode[i], (int)machineCode[i], i, i);
            }
        }
    }


    const std::vector<std::string>& GetSourceInstructions() const noexcept
    {
        return m_SourceCode;
    }
};


#endif // DISASSEMBLER_H