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
    std::string m_ErrorMsg;
    std::vector<std::pair<std::uint16_t, std::string>> m_SourceCode;

    const std::unordered_map<OpcodeMC, std::string_view> m_OpcodeToStringMap = {
        { OpcodeMC::MOV_IMM_TO_REG  , "MOV" },
        { OpcodeMC::ADD_IMM_TO_REG  , "ADD" },
        { OpcodeMC::SUB_IMM_TO_REG  , "SUB" },
        { OpcodeMC::CMP_IMM_TO_REG  , "CMP" },
        { OpcodeMC::MOV_REG_TO_REG  , "MOV" },
        { OpcodeMC::ADD_REG_TO_REG  , "ADD" },
        { OpcodeMC::SUB_REG_TO_REG  , "SUB" },
        { OpcodeMC::CMP_REG_TO_REG  , "CMP" },
        { OpcodeMC::JMP_REG         , "JMP" },
        { OpcodeMC::JMP_LABEL       , "JMP" },
        { OpcodeMC::JE_REG          , "JE " },
        { OpcodeMC::JE_LABEL        , "JE " },
        { OpcodeMC::BRK             , "BRK" },
        { OpcodeMC::HALT            , "HALT" },
        { OpcodeMC::LOAD_ADD_TO_REG , "LOAD" },
        { OpcodeMC::LOAD_REG_TO_REG , "LOAD" },
        { OpcodeMC::STORE_REG_TO_ADD, "STORE" },
        { OpcodeMC::STORE_REG_TO_REG, "STORE" },
    };


    const std::unordered_map<std::uint8_t, std::string_view> m_RegisterToStringMap = {
        { 0  , "R0" },
        { 1  , "R1" },
        { 2  , "R2" },
        { 3  , "R3" },
        { 4  , "R4" },
        { 5  , "R5" },
        { 6  , "R6" },
        { 7  , "R7" }
    };
private:
    std::string DisassembleOperands(OpcodeMC opcode, const std::vector<std::uint8_t>& machineCode, std::uint16_t index) const;
public:
    void Disassemble(const std::vector<std::uint8_t>& machineCode);


    inline const std::vector<std::pair<std::uint16_t, std::string>>& GetSourceInstructions() const noexcept
    {
        return m_SourceCode;
    }


    inline const std::string& GetErrorMsg() const noexcept
    {
        return m_ErrorMsg;
    }
};


#endif // DISASSEMBLER_H