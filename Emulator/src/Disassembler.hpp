#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <vector>
#include <string>
#include <cstdint>
#include <utility>
#include <unordered_map>

#include "../../Assembler/lib/src/Assembler.hpp"

class Disassembler
{
private:
    std::vector<std::pair<std::uint16_t, std::string>> m_SourceCode;

    const std::unordered_map<OpcodeMC, std::string_view> m_OpcodeToStringMap = {
        { OpcodeMC::MOV_IMM_TO_REG , "MOV" },
        { OpcodeMC::ADD_IMM_TO_REG , "ADD" },
        { OpcodeMC::SUB_IMM_TO_REG , "SUB" },
        { OpcodeMC::CMP_IMM_TO_REG , "CMP" },
        { OpcodeMC::MOV_REG_TO_REG , "MOV" },
        { OpcodeMC::ADD_REG_TO_REG , "ADD" },
        { OpcodeMC::SUB_REG_TO_REG , "SUB" },
        { OpcodeMC::CMP_REG_TO_REG , "CMP" },
        { OpcodeMC::JMP_REG        , "JMP" },
        { OpcodeMC::JMP_LABEL      , "JMP" },
        { OpcodeMC::JE_REG         , "JE " },
        { OpcodeMC::JE_LABEL       , "JE " },
        { OpcodeMC::HLT            , "HTL" },
        { OpcodeMC::LOAD_ADD_TO_REG, "LOAD" },
        { OpcodeMC::LOAD_REG_TO_REG, "LOAD" },
        { OpcodeMC::STORE          , "STORE" },
    };
private:
    std::string DisassembleOperands(OpcodeMC opcode, const std::vector<std::uint8_t>& machineCode, size_t index) const;
public:
    explicit Disassembler(const std::vector<std::uint8_t>& machineCode);


    inline const std::vector<std::pair<std::uint16_t, std::string>>& GetSourceInstructions() const noexcept
    {
        return m_SourceCode;
    }
};


#endif // DISASSEMBLER_H