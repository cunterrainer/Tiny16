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

void CPU::Instruction_JE_REG(OpcodeMC opcode)
{
    if (m_Registers[RF] == Flags::Equal)
    {
        const Register reg = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
        m_ProgramCounter = m_Registers[reg];
    }
}

void CPU::Instruction_JE_LABEL(OpcodeMC opcode)
{
    if (m_Registers[RF] == Flags::Equal)
        m_ProgramCounter = m_Prom.Read16(m_ProgramCounter + OpcodeOffset);
}

void CPU::Instruction_HLT(OpcodeMC)
{
    m_ExecutionMode = false; // Stay stuck at this instructions, NOT a bug
}

void CPU::Instruction_LOAD(OpcodeMC opcode)
{
    const std::uint16_t imm = m_Prom.Read16(m_ProgramCounter + OpcodeOffset);
    const Register reg = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + ImmediateOffset);

    m_Registers[reg] = m_Ram.GetMemory(imm);
    m_ProgramCounter += s_InstructionIRSizeMap.at(opcode);
}

void CPU::Instruction_STORE(OpcodeMC opcode)
{
    const Register reg = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const std::uint16_t imm = m_Prom.Read16(m_ProgramCounter + OpcodeOffset + RegisterOffset);

    m_Ram.SetMemory(imm, m_Registers[reg]);
    m_ProgramCounter += s_InstructionIRSizeMap.at(opcode);
}


void CPU::Clock()
{
    // TODO error checking if opcode exists
    const OpcodeMC opcode = (OpcodeMC)m_Prom.Read(m_ProgramCounter);
    (this->*m_InstructionFunctionTable.at(opcode))(opcode);
}