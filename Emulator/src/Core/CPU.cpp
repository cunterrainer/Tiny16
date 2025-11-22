#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <unordered_map>

#include "../../Assembler/lib/src/Assembler.hpp"
#include "../../Assembler/lib/src/Intermediate.hpp"

#include "CPU.hpp"


void CPU::Instruction_MOV_IMM_TO_REG(OpcodeMC opcode)
{
    const std::uint16_t imm = m_Prom.Read16(m_ProgramCounter + OpcodeOffset);
    const Register reg = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + ImmediateOffset);

    m_Registers[reg] = imm;
    m_ProgramCounter += s_InstructionIRSizeMap.at(opcode);
}

void CPU::Instruction_ADD_IMM_TO_REG(OpcodeMC opcode)
{
    const std::uint16_t imm = m_Prom.Read16(m_ProgramCounter + OpcodeOffset);
    const Register reg = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + ImmediateOffset);

    m_Registers[reg] += imm;
    m_ProgramCounter += s_InstructionIRSizeMap.at(opcode);
}

void CPU::Instruction_SUB_IMM_TO_REG(OpcodeMC opcode)
{
    const std::uint16_t imm = m_Prom.Read16(m_ProgramCounter + OpcodeOffset);
    const Register reg = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + ImmediateOffset);

    m_Registers[reg] -= imm;
    m_ProgramCounter += s_InstructionIRSizeMap.at(opcode);
}

void CPU::Instruction_CMP_IMM_TO_REG(OpcodeMC opcode)
{
    const std::uint16_t imm = m_Prom.Read16(m_ProgramCounter + OpcodeOffset);
    const Register reg = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + ImmediateOffset);

    m_Registers[RF] = (imm == m_Registers[reg]) ? Flags::Equal
                    : (imm > m_Registers[reg]) ? Flags::Greater
                    : Flags::Less;
    m_ProgramCounter += s_InstructionIRSizeMap.at(opcode);
}

void CPU::Instruction_MOV_REG_TO_REG(OpcodeMC opcode)
{
    const Register regSrc = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const Register regDst = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + RegisterOffset);
    m_Registers[regDst] = m_Registers[regSrc];
    m_ProgramCounter += s_InstructionIRSizeMap.at(opcode);
}

void CPU::Instruction_ADD_REG_TO_REG(OpcodeMC opcode)
{
    const Register regSrc = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const Register regDst = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + RegisterOffset);
    m_Registers[regDst] += m_Registers[regSrc];
    m_ProgramCounter += s_InstructionIRSizeMap.at(opcode);
}

void CPU::Instruction_SUB_REG_TO_REG(OpcodeMC opcode)
{
    const Register regSrc = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const Register regDst = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + RegisterOffset);
    m_Registers[regDst] -= m_Registers[regSrc];
    m_ProgramCounter += s_InstructionIRSizeMap.at(opcode);
}

void CPU::Instruction_CMP_REG_TO_REG(OpcodeMC opcode)
{
    const Register reg1 = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const Register reg2 = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + RegisterOffset);

    m_Registers[RF] = (m_Registers[reg1] == m_Registers[reg2]) ? Flags::Equal
                    : (m_Registers[reg1] > m_Registers[reg2]) ? Flags::Greater
                    : Flags::Less;

    m_ProgramCounter += s_InstructionIRSizeMap.at(opcode);
}

void CPU::Instruction_JMP_REG(OpcodeMC)
{
    const Register reg = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    m_ProgramCounter = m_Registers[reg];
}

void CPU::Instruction_JMP_LABEL(OpcodeMC)
{
    const std::uint16_t addr = m_Prom.Read16(m_ProgramCounter + OpcodeOffset);
    m_ProgramCounter = addr;
}

void CPU::Instruction_JE_REG(OpcodeMC opcode)
{
    if (m_Registers[RF] == Flags::Equal)
    {
        const Register reg = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
        m_ProgramCounter = m_Registers[reg];
    }
    else
        m_ProgramCounter += s_InstructionIRSizeMap.at(opcode);
}

void CPU::Instruction_JE_LABEL(OpcodeMC opcode)
{
    if (m_Registers[RF] == Flags::Equal)
        m_ProgramCounter = m_Prom.Read16(m_ProgramCounter + OpcodeOffset);
    else
        m_ProgramCounter += s_InstructionIRSizeMap.at(opcode);
}

void CPU::Instruction_HLT(OpcodeMC)
{
    m_ExecutionMode = false; // Stay stuck at this instructions, NOT a bug
}

void CPU::Instruction_LOAD_ADD_TO_REG(OpcodeMC opcode)
{
    const std::uint16_t imm = m_Prom.Read16(m_ProgramCounter + OpcodeOffset);
    const Register reg = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + ImmediateOffset);

    // We work in little endian
    m_Registers[reg] = (m_Ram.GetMemory(imm + 1) << 8) | m_Ram.GetMemory(imm);
    m_ProgramCounter += s_InstructionIRSizeMap.at(opcode);
}

void CPU::Instruction_LOAD_REG_TO_REG(OpcodeMC opcode)
{
    const Register regSrcAddr = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const Register regDst = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + RegisterOffset);

    // We work in little endian
    m_Registers[regDst] = (m_Ram.GetMemory(m_Registers[regSrcAddr] + 1) << 8) | m_Ram.GetMemory(m_Registers[regSrcAddr]);
    m_ProgramCounter += s_InstructionIRSizeMap.at(opcode);
}

void CPU::Instruction_STORE_REG_TO_ADD(OpcodeMC opcode)
{
    const Register reg = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const std::uint16_t imm = m_Prom.Read16(m_ProgramCounter + OpcodeOffset + RegisterOffset);

    // We work in little endian
    const std::uint8_t lowerHalf = static_cast<std::uint8_t>(m_Registers[reg]);
    const std::uint8_t upperHalf = static_cast<std::uint8_t>(m_Registers[reg] >> 8);

    m_Ram.SetMemory(imm, lowerHalf);
    m_Ram.SetMemory(imm + 1, upperHalf);
    m_ProgramCounter += s_InstructionIRSizeMap.at(opcode);
}

void CPU::Instruction_STORE_REG_TO_REG(OpcodeMC opcode)
{
    const Register regValue = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const Register regDstAddr = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + RegisterOffset);

    // We work in little endian
    const std::uint8_t lowerHalf = static_cast<std::uint8_t>(m_Registers[regValue]);
    const std::uint8_t upperHalf = static_cast<std::uint8_t>(m_Registers[regValue] >> 8);

    m_Ram.SetMemory(m_Registers[regDstAddr], lowerHalf);
    m_Ram.SetMemory(m_Registers[regDstAddr] + 1, upperHalf);
    m_ProgramCounter += s_InstructionIRSizeMap.at(opcode);
}


void CPU::Clock()
{
    /*
        Decided to use a massive switch statement due to a 20 - 50 % performance
        increase compared to the lookup table
    */
    const OpcodeMC opcode = (OpcodeMC)m_Prom.Read(m_ProgramCounter);
    switch (opcode)
    {
    case OpcodeIR::MOV_IMM_TO_REG:      return Instruction_MOV_IMM_TO_REG(opcode);
    case OpcodeIR::ADD_IMM_TO_REG:      return Instruction_ADD_IMM_TO_REG(opcode);
    case OpcodeIR::SUB_IMM_TO_REG:      return Instruction_SUB_IMM_TO_REG(opcode);
    case OpcodeIR::CMP_IMM_TO_REG:      return Instruction_CMP_IMM_TO_REG(opcode);

    case OpcodeIR::MOV_REG_TO_REG:      return Instruction_MOV_REG_TO_REG(opcode);
    case OpcodeIR::ADD_REG_TO_REG:      return Instruction_ADD_REG_TO_REG(opcode);
    case OpcodeIR::SUB_REG_TO_REG:      return Instruction_SUB_REG_TO_REG(opcode);
    case OpcodeIR::CMP_REG_TO_REG:      return Instruction_CMP_REG_TO_REG(opcode);

    case OpcodeIR::JMP_REG:             return Instruction_JMP_REG(opcode);
    case OpcodeIR::JMP_LABEL:           return Instruction_JMP_LABEL(opcode);
    case OpcodeIR::JE_REG:              return Instruction_JE_REG(opcode);
    case OpcodeIR::JE_LABEL:            return Instruction_JE_LABEL(opcode);

    case OpcodeIR::HLT:                 return Instruction_HLT(opcode);

    case OpcodeIR::LOAD_ADD_TO_REG:     return Instruction_LOAD_ADD_TO_REG(opcode);
    case OpcodeIR::LOAD_REG_TO_REG:     return Instruction_LOAD_REG_TO_REG(opcode);
    case OpcodeIR::STORE_REG_TO_ADD:    return Instruction_STORE_REG_TO_ADD(opcode);
    case OpcodeIR::STORE_REG_TO_REG:    return Instruction_STORE_REG_TO_REG(opcode);

    default:
        m_ErrorMsg = "Execution failed: unknown instruction or memory read error. Did you miss a HLT instruction?";
        m_ExecutionMode = false;
        return;
    }
}


CPU::CPU(CPU&& cpu) noexcept : m_Prom(cpu.m_Prom), m_Ram(cpu.m_Ram)
{
    m_ExecutionMode = cpu.m_ExecutionMode;
    m_ProgramCounter = cpu.m_ProgramCounter;
    m_Registers = cpu.m_Registers;
}


CPU::CPU(const CPU& cpu) : m_Prom(cpu.m_Prom), m_Ram(cpu.m_Ram)
{
    m_ExecutionMode = cpu.m_ExecutionMode;
    m_ProgramCounter = cpu.m_ProgramCounter;
    m_Registers = cpu.m_Registers;
}