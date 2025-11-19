#include <vector>
#include <string>
#include <cstdint>
#include <unordered_map>

#include "../../Assembler/lib/src/Assembler.hpp"
#include "../../Assembler/lib/src/Intermediate.hpp"

#include "CPU.hpp"
#include "Log.hpp"
#include "Utility.hpp"


inline std::uint16_t CPU::GetImmediate16(const std::uint8_t* ptr) const noexcept
{
    uint16_t imm = *reinterpret_cast<const std::uint16_t*>(ptr);

    // We emulate a little endian CPU
    if constexpr (Util::Bytes::HostIsBigEndian())
    {
        return Util::Bytes::SwapEndian16(imm);
    }
    else
    {
        return imm;
    }
}


//void CPU::Execute(const std::vector<std::uint8_t>& code) noexcept
//{
//    for (std::size_t i = 0; i < code.size();)
//    {
//        // TODO add flags e.g. overflow to add
//        switch (static_cast<Instruction>(code[i]))
//        {
//        case Instruction::MOVI: // mov (16bit) reg
//        {
//            ERR_IF(i + 4 > code.size(), "MOVI: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 4, code.size() - i, i);
//
//            const std::uint16_t imm = GetImmediate16(&code[i + 1]);
//            const Register reg = static_cast<Register>(code[i + 3]);
//            ERR_IF(reg >= Register::RF, "MOVI: Illegal register used: 0x{:X}", static_cast<std::size_t>(reg));
//            m_Registers[reg] = imm;
//            i += 4;
//            break;
//        }
//        case Instruction::MOVR: // mov reg reg
//        {
//            ERR_IF(i + 3 > code.size(), "MOVR: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 3, code.size() - i, i);
//
//            const std::uint8_t src = code[i + 1];
//            const std::uint8_t dest = code[i + 2];
//            ERR_IF(src >= Register::RF, "MOVR: Source register doesn't exist: 0x{:X}", src);
//            ERR_IF(dest >= Register::RF, "MOVR: Destination register doesn't exist: 0x{:X}", dest);
//            m_Registers[dest] = m_Registers[src];
//            i += 3;
//            break;
//        }
//        case Instruction::ADDI: // add (16bit) reg
//        {
//            ERR_IF(i + 4 > code.size(), "ADDI: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 4, code.size() - i, i);
//
//            const std::uint16_t imm = GetImmediate16(&code[i + 1]);
//            const Register reg = static_cast<Register>(code[i + 3]);
//            ERR_IF(reg >= Register::RF, "ADDI: Illegal register used: 0x{:X}", static_cast<std::size_t>(reg));
//            m_Registers[reg] += imm;
//            i += 4;
//            break;
//        }
//        case Instruction::ADDR: // add reg reg
//        {
//            ERR_IF(i + 3 > code.size(), "ADDR: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 3, code.size() - i, i);
//
//            const std::uint8_t src = code[i + 1];
//            const std::uint8_t dest = code[i + 2];
//            ERR_IF(src >= Register::RF, "ADDR: Source register doesn't exist: 0x{:X}", src);
//            ERR_IF(dest >= Register::RF, "ADDR: Destination register doesn't exist: 0x{:X}", dest);
//            m_Registers[dest] += m_Registers[src];
//            i += 3;
//            break;
//        }
//        case Instruction::SUBI: // sub (16bit) reg
//        {
//            ERR_IF(i + 4 > code.size(), "SUBI: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 4, code.size() - i, i);
//
//            const std::uint16_t imm = GetImmediate16(&code[i + 1]);
//            const Register reg = static_cast<Register>(code[i + 3]);
//            ERR_IF(reg >= Register::RF, "SUBI: Illegal register used: 0x{:X}", static_cast<std::size_t>(reg));
//            m_Registers[reg] -= imm;
//            i += 4;
//            break;
//        }
//        case Instruction::SUBR: // sub reg reg
//        {
//            ERR_IF(i + 3 > code.size(), "SUBR: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 3, code.size() - i, i);
//
//            const std::uint8_t src = code[i + 1];
//            const std::uint8_t dest = code[i + 2];
//            ERR_IF(src >= Register::RF, "SUBR: Source register doesn't exist: 0x{:X}", src);
//            ERR_IF(dest >= Register::RF, "SUBR: Destination register doesn't exist: 0x{:X}", dest);
//            m_Registers[dest] -= m_Registers[src];
//            i += 3;
//            break;
//        }
//        case Instruction::MULI: // mul (16bit) reg
//        {
//            ERR_IF(i + 4 > code.size(), "MULI: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 4, code.size() - i, i);
//
//            const std::uint16_t imm = GetImmediate16(&code[i + 1]);
//            const Register reg = static_cast<Register>(code[i + 3]);
//            ERR_IF(reg >= Register::RF, "MULI: Illegal register used: 0x{:X}", static_cast<std::size_t>(reg));
//            m_Registers[reg] *= imm;
//            i += 4;
//            break;
//        }
//        case Instruction::MULR: // mul reg reg
//        {
//            ERR_IF(i + 3 > code.size(), "MULR: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 3, code.size() - i, i);
//
//            const std::uint8_t src = code[i + 1];
//            const std::uint8_t dest = code[i + 2];
//            ERR_IF(src >= Register::RF, "MULR: Source register doesn't exist: 0x{:X}", src);
//            ERR_IF(dest >= Register::RF, "MULR: Destination register doesn't exist: 0x{:X}", dest);
//            m_Registers[dest] *= m_Registers[src];
//            i += 3;
//            break;
//        }
//        case Instruction::IMULI: // imul (16bit) reg
//        {
//            ERR_IF(i + 4 > code.size(), "IMULI: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 4, code.size() - i, i);
//
//            const std::uint16_t imm = GetImmediate16(&code[i + 1]);
//            const Register reg = static_cast<Register>(code[i + 3]);
//            ERR_IF(reg >= Register::RF, "IMULI: Illegal register used: 0x{:X}", static_cast<std::size_t>(reg));
//            m_Registers[reg] = static_cast<std::int16_t>(m_Registers[reg]) * static_cast<std::int16_t>(imm);
//            i += 4;
//            break;
//        }
//        case Instruction::IMULR: // imul reg reg
//        {
//            ERR_IF(i + 3 > code.size(), "IMULR: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 3, code.size() - i, i);
//
//            const std::uint8_t src = code[i + 1];
//            const std::uint8_t dest = code[i + 2];
//            ERR_IF(src >= Register::RF, "IMULR: Source register doesn't exist: 0x{:X}", src);
//            ERR_IF(dest >= Register::RF, "IMULR: Destination register doesn't exist: 0x{:X}", dest);
//            m_Registers[dest] = static_cast<std::int16_t>(m_Registers[dest]) * static_cast<std::int16_t>(m_Registers[src]);
//            i += 3;
//            break;
//        }
//        case Instruction::DIVI: // div (16bit) reg
//        {
//            ERR_IF(i + 4 > code.size(), "DIVI: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 4, code.size() - i, i);
//
//            const std::uint16_t imm = GetImmediate16(&code[i + 1]);
//            const Register reg = static_cast<Register>(code[i + 3]);
//            ERR_IF(reg >= Register::RF, "DIVI: Illegal register used: 0x{:X}", static_cast<std::size_t>(reg));
//
//            [[likely]] if (imm != 0)
//            {
//                // otherwise we may override R0 for the second division
//                const uint16_t r0tmp = m_Registers[reg] / imm;
//                const uint16_t r1tmp = m_Registers[reg] % imm;
//                m_Registers[Register::R0] = r0tmp;
//                m_Registers[Register::R1] = r1tmp;
//            }
//            i += 4;
//            break;
//        }
//        case Instruction::DIVR: // div reg reg
//        {
//            ERR_IF(i + 3 > code.size(), "DIVR: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 3, code.size() - i, i);
//
//            const std::uint8_t src = code[i + 1];
//            const std::uint8_t dest = code[i + 2];
//            ERR_IF(src >= Register::RF, "DIVR: Source register doesn't exist: 0x{:X}", src);
//            ERR_IF(dest >= Register::RF, "DIVR: Destination register doesn't exist: 0x{:X}", dest);
//
//            [[likely]] if (m_Registers[src] != 0)
//            {
//                // otherwise the my override R0 for the second division
//                const std::uint16_t r0tmp = m_Registers[dest] / m_Registers[src];
//                const std::uint16_t r1tmp = m_Registers[dest] % m_Registers[src];
//                m_Registers[Register::R0] = r0tmp;
//                m_Registers[Register::R1] = r1tmp;
//            }
//            i += 3;
//            break;
//        }
//        case Instruction::IDIVI: // idiv (16bit) reg
//        {
//            ERR_IF(i + 4 > code.size(), "IDIVI: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 4, code.size() - i, i);
//
//            const std::uint16_t imm = GetImmediate16(&code[i + 1]);
//            const Register reg = static_cast<Register>(code[i + 3]);
//            ERR_IF(reg >= Register::RF, "IDIVI: Illegal register used: 0x{:X}", static_cast<std::size_t>(reg));
//            m_Registers[reg] = static_cast<std::int16_t>(m_Registers[reg]) * static_cast<std::int16_t>(imm);
//
//            [[likely]] if (imm != 0)
//            {
//                // otherwise we may override R0 for the second division
//                const std::int16_t r0tmp = static_cast<std::int16_t>(m_Registers[reg]) / static_cast<std::int16_t>(imm);
//                const std::int16_t r1tmp = static_cast<std::int16_t>(m_Registers[reg]) % static_cast<std::int16_t>(imm);
//                m_Registers[Register::R0] = r0tmp;
//                m_Registers[Register::R1] = r1tmp;
//            }
//
//            i += 4;
//            break;
//        }
//        case Instruction::IDIVR: // idiv reg reg
//        {
//            ERR_IF(i + 3 > code.size(), "IDIVR: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 3, code.size() - i, i);
//
//            const std::uint8_t src = code[i + 1];
//            const std::uint8_t dest = code[i + 2];
//            ERR_IF(src >= Register::RF, "IDIVR: Source register doesn't exist: 0x{:X}", src);
//            ERR_IF(dest >= Register::RF, "IDIVR: Destination register doesn't exist: 0x{:X}", dest);
//
//            [[likely]] if (m_Registers[src] != 0)
//            {
//                // otherwise we may override R0 for the second division
//                const std::int16_t r0tmp = static_cast<std::int16_t>(m_Registers[dest]) / static_cast<std::int16_t>(m_Registers[src]);
//                const std::int16_t r1tmp = static_cast<std::int16_t>(m_Registers[dest]) % static_cast<std::int16_t>(m_Registers[src]);
//                m_Registers[Register::R0] = r0tmp;
//                m_Registers[Register::R1] = r1tmp;
//            }
//
//            i += 3;
//            break;
//        }
//        case Instruction::EXIT:
//        {
//            return;
//        }
//        default:
//            ERR("Unsupported instruction used: 0x{:X} ({})", static_cast<std::size_t>(code[i]), static_cast<std::size_t>(code[i]));
//            break;
//        }
//    }
//}


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
    // TODO
    m_ProgramCounter += s_InstructionIRSizeMap.at(opcode);
}

void CPU::Instruction_STORE(OpcodeMC opcode)
{
    // TODO
    m_ProgramCounter += s_InstructionIRSizeMap.at(opcode);
}


void CPU::Clock2()
{
    // TODO error checking if opcode exists
    const OpcodeMC opcode = (OpcodeMC)m_Prom.Read(m_ProgramCounter);
    (this->*m_InstructionFunctionTable.at(opcode))(opcode);
}


std::string CPU::Clock() noexcept
{
    const Instruction instruction = (Instruction)m_Prom.Read(m_ProgramCounter);

    switch (instruction)
    {
    case Instruction::MOVI: // imm16, reg
    {
        const std::uint16_t imm = m_Prom.Read16(m_ProgramCounter + 1);
        const Register reg = (Register)m_Prom.Read(m_ProgramCounter + 3);
        m_Registers[reg] = imm;
        m_ProgramCounter += 4;
        return std::format("MOV 0x{:04X}, R{}", imm, (uint16_t)reg);
    }
    case Instruction::MOVR: // reg, reg
    {
        const Register regSrc = (Register)m_Prom.Read(m_ProgramCounter + 1);
        const Register regDst = (Register)m_Prom.Read(m_ProgramCounter + 2);
        m_Registers[regDst] = m_Registers[regSrc];
        m_ProgramCounter += 3;
        return std::format("MOV R{}, R{}", (uint16_t)regSrc, (uint16_t)regDst);
    }
    case Instruction::ADDI: // imm16, reg
    {
        const std::uint16_t imm = m_Prom.Read16(m_ProgramCounter + 1);
        const Register reg = (Register)m_Prom.Read(m_ProgramCounter + 3);
        m_Registers[reg] += imm;
        m_ProgramCounter += 4;
        return std::format("ADD 0x{:04X}, R{}", imm, (uint16_t)reg);
    }
    case Instruction::ADDR: // reg, reg
    {
        const Register regSrc = (Register)m_Prom.Read(m_ProgramCounter + 1);
        const Register regDst = (Register)m_Prom.Read(m_ProgramCounter + 2);
        m_Registers[regDst] += m_Registers[regSrc];
        m_ProgramCounter += 3;
        return std::format("ADD R{}, R{}", (uint16_t)regSrc, (uint16_t)regDst);
    }
    case Instruction::SUBI: // imm16, reg
    {
        const std::uint16_t imm = m_Prom.Read16(m_ProgramCounter + 1);
        const Register reg = (Register)m_Prom.Read(m_ProgramCounter + 3);
        m_Registers[reg] -= imm;
        m_ProgramCounter += 4;
        return std::format("SUB 0x{:04X}, R{}", imm, (uint16_t)reg);
    }
    case Instruction::SUBR: // reg, reg
    {
        const Register regSrc = (Register)m_Prom.Read(m_ProgramCounter + 1);
        const Register regDst = (Register)m_Prom.Read(m_ProgramCounter + 2);
        m_Registers[regDst] -= m_Registers[regSrc];
        m_ProgramCounter += 3;
        return std::format("SUB R{}, R{}", (uint16_t)regSrc, (uint16_t)regDst);
    }
    case Instruction::JMP:
    {
        const std::uint16_t dest = m_Prom.Read16(m_ProgramCounter + 1);
        m_ProgramCounter = dest;
        return std::format("JMP 0x{:04X}", dest);
    }
    case Instruction::JE:
    {
        const std::uint16_t dest = m_Prom.Read16(m_ProgramCounter + 1);
        if (m_Registers[Register::RF] & Flags::Equal)
            m_ProgramCounter = dest;
        return std::format("JE 0x{:04X}", dest);
    }
    case Instruction::CMPI:
    {
        const std::uint16_t imm = m_Prom.Read16(m_ProgramCounter + 1);
        const Register reg = (Register)m_Prom.Read(m_ProgramCounter + 3);

        const std::int32_t res = m_Registers[reg] - imm;

        if (res == 0)
            m_Registers[RF] | Flags::Equal;

        else if (res < 0)
            m_Registers[RF] | Flags::Greater;

        else if (res > 0)
            m_Registers[RF] | Flags::Less;

        m_ProgramCounter += 4;
        return std::format("CMP 0x{:04X}, R{}", imm, (uint16_t)reg);
    }
    case Instruction::CMPR:
    {
        const Register regSrc = (Register)m_Prom.Read(m_ProgramCounter + 1);
        const Register regDst = (Register)m_Prom.Read(m_ProgramCounter + 2);

        const std::int32_t res = m_Registers[regDst] - m_Registers[regSrc];

        if (res == 0)
            m_Registers[RF] | Flags::Equal;

        else if (res < 0)
            m_Registers[RF] | Flags::Greater;

        else if (res > 0)
            m_Registers[RF] | Flags::Less;

        m_ProgramCounter += 4;
        return std::format("CMP R{}, R{}", (uint16_t)regSrc, (uint16_t)regSrc);
    }
    case Instruction::HLT:
    {
        m_ProgramCounter = 0xFFFF;
        return "HLT";
    }
    default:
        ERR("Instruction not supported, Opcode: 0x{:X}", (std::uint16_t)instruction);
        m_ProgramCounter += 1;
        return "Unknown";
    }
}