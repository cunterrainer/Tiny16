#include <stdexcept>
#include <vector>
#include <limits>
#include <cstdint>
#include <variant>

#include "Assembler.hpp"
#include "Intermediate.hpp"

#include "Utility/Result.hpp"

InstructionMC AssembleInstruction(const InstructionIR& instrIr, const std::unordered_map<std::string, std::uint32_t>& labelAddress)
{
    InstructionMC instrMc;
    instrMc.opcode = static_cast<std::uint8_t>(instrIr.opcode);

    switch (instrIr.opcode)
    {
    case OpcodeIR::MOV_IMM_TO_REG:
    case OpcodeIR::ADD_IMM_TO_REG:
    case OpcodeIR::SUB_IMM_TO_REG:
    case OpcodeIR::CMP_IMM_TO_REG:
    case OpcodeIR::LOAD_ADD_TO_REG:
        instrMc.op1.type = OperandTypeMC::Intermediate;
        instrMc.op1.value = std::get<std::uint16_t>(instrIr.op1.value);
        break;
    case OpcodeIR::JE_REG:
    case OpcodeIR::JMP_REG:
    case OpcodeIR::MOV_REG_TO_REG:
    case OpcodeIR::ADD_REG_TO_REG:
    case OpcodeIR::SUB_REG_TO_REG:
    case OpcodeIR::CMP_REG_TO_REG:
    case OpcodeIR::LOAD_REG_TO_REG:
    case OpcodeIR::STORE_REG_TO_ADD:
    case OpcodeIR::STORE_REG_TO_REG:
        instrMc.op1.type = OperandTypeMC::Register;
        instrMc.op1.value = std::get<std::uint8_t>(instrIr.op1.value);
        break;
    case OpcodeIR::JE_LABEL:
    case OpcodeIR::JMP_LABEL:
        instrMc.op1.type = OperandTypeMC::Intermediate;
        instrMc.op1.value = (std::uint16_t)labelAddress.at(std::get<std::string>(instrIr.op1.value));
        break;
    case OpcodeIR::HLT:
        instrMc.op1.type = OperandTypeMC::None;
        break;
    default:
        throw std::logic_error("AssembleInstruction operand 1 invalid");
        break;
    }

    switch (instrIr.opcode)
    {
    case OpcodeIR::MOV_IMM_TO_REG:
    case OpcodeIR::ADD_IMM_TO_REG:
    case OpcodeIR::SUB_IMM_TO_REG:
    case OpcodeIR::CMP_IMM_TO_REG:
    case OpcodeIR::MOV_REG_TO_REG:
    case OpcodeIR::ADD_REG_TO_REG:
    case OpcodeIR::SUB_REG_TO_REG:
    case OpcodeIR::CMP_REG_TO_REG:
    case OpcodeIR::LOAD_ADD_TO_REG:
    case OpcodeIR::LOAD_REG_TO_REG:
    case OpcodeIR::STORE_REG_TO_REG:
        instrMc.op2.type = OperandTypeMC::Register;
        instrMc.op2.value = std::get<std::uint8_t>(instrIr.op2.value);
        break;
    case OpcodeIR::STORE_REG_TO_ADD:
        instrMc.op2.type = OperandTypeMC::Intermediate;
        instrMc.op2.value = std::get<std::uint16_t>(instrIr.op2.value);
        break;
    case OpcodeIR::HLT:
    case OpcodeIR::JMP_REG:
    case OpcodeIR::JMP_LABEL:
    case OpcodeIR::JE_REG:
    case OpcodeIR::JE_LABEL:
        instrMc.op2.type = OperandTypeMC::None;
        break;
    default:
        throw std::logic_error("AssembleInstruction operand 2 invalid");
        break;
    }

    return instrMc;
}


std::uint32_t ResolveLabelAddress(std::string_view label, const std::vector<InstructionIR>& instr, std::uint32_t offset)
{
    std::uint32_t address = offset;
    for (const auto& i : instr)
    {
        if (label == i.label)
        {
            if (address > std::numeric_limits<std::uint16_t>::max())
            {
                throw std::logic_error(std::format("ResolveLabelAddress: Address for label {} = {}, greater than allowed for 16 bit architecture", label, address));
            }
            return address;
        }
        address += i.size;
    }
    throw std::logic_error(std::format("ResolveLabelAddress: Label doesn't exist: {}", label));
}


Result<std::vector<InstructionMC>> AssembleInstructions(const std::vector<InstructionIR>& instr)
{
    std::unordered_map<std::string, std::uint32_t> labelAddress;

    InstructionIR jmpToMain;
    jmpToMain.opcode = OpcodeIR::JMP_LABEL;
    jmpToMain.op1.type = OperandTypeIR::Label;
    jmpToMain.op1.value = "main";
    jmpToMain.size = s_InstructionIRSizeMap.at(jmpToMain.opcode);

    for (const auto& i : instr)
    {
        if (!i.label.empty())
            labelAddress[i.label] = ResolveLabelAddress(i.label, instr, jmpToMain.size);
    }

    // Check if main exists
    if (!labelAddress.contains("main"))
    {
        return Err("Linker error: Failed to find main label, entry point has to be set via the main label");
    }

    std::vector<InstructionMC> instructions;
    instructions.reserve(instr.size() + 1);
    instructions.push_back(AssembleInstruction(jmpToMain, labelAddress));

    for (const auto& i : instr)
    {
        instructions.push_back(AssembleInstruction(i, labelAddress));
    }

    return instructions;
}
