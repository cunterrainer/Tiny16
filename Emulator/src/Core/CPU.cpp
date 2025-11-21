#include <vector>
#include <string>
#include <cstdint>
#include <unordered_map>

#include "../../Assembler/lib/src/Assembler.hpp"
#include "../../Assembler/lib/src/Intermediate.hpp"

#include "CPU.hpp"
#include "Log.hpp"
#include "Utility.hpp"


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

    if (m_Registers[reg] == imm)
        m_Registers[RF] = Flags::Equal;
    else if (imm > m_Registers[reg])
        m_Registers[RF] = Flags::Greater;
    else if (imm < m_Registers[reg])
        m_Registers[RF] = Flags::Less;

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

    if (m_Registers[reg1] == m_Registers[reg2])
        m_Registers[RF] = Flags::Equal;
    else if (m_Registers[reg1] > m_Registers[reg2])
        m_Registers[RF] = Flags::Greater;
    else if (m_Registers[reg1] < m_Registers[reg2])
        m_Registers[RF] = Flags::Less;

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

void CPU::Instruction_JE_REG(OpcodeMC)
{
    if (m_Registers[RF] == Flags::Equal)
    {
        const Register reg = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
        m_ProgramCounter = m_Registers[reg];
    }
}

void CPU::Instruction_JE_LABEL(OpcodeMC)
{
    if (m_Registers[RF] == Flags::Equal)
        m_ProgramCounter = m_Prom.Read16(m_ProgramCounter + OpcodeOffset);
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
    // TODO error checking if opcode exists
    const OpcodeMC opcode = (OpcodeMC)m_Prom.Read(m_ProgramCounter);
    (this->*m_InstructionFunctionTable.at(opcode))(opcode);
}


CPU::CPU(CPU&& cpu) : m_Prom(cpu.m_Prom), m_Ram(cpu.m_Ram)
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