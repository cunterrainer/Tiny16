#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <unordered_map>

#include "../../Assembler/lib/src/Assembler.hpp"
#include "../../Assembler/lib/src/Intermediate.hpp"

#include "CPU.hpp"


// ----------------------- Data storage -----------------------
void CPU::Instruction_MOV_IMM_TO_REG()
{
    const std::uint16_t imm = m_Prom.Read16(m_ProgramCounter + OpcodeOffset);
    const Register reg = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + ImmediateOffset);

    m_Registers[reg] = imm;
}

void CPU::Instruction_MOV_REG_TO_REG()
{
    const Register regSrc = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const Register regDst = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + RegisterOffset);
    m_Registers[regDst] = m_Registers[regSrc];
}

void CPU::Instruction_LOAD_ADD_TO_REG()
{
    const std::uint16_t imm = m_Prom.Read16(m_ProgramCounter + OpcodeOffset);
    const Register reg = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + ImmediateOffset);

    // We work in little endian
    m_Registers[reg] = (m_Ram.GetMemory(imm + 1) << 8) | m_Ram.GetMemory(imm);
}

void CPU::Instruction_LOAD_REG_TO_REG()
{
    const Register regSrcAddr = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const Register regDst = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + RegisterOffset);

    // We work in little endian
    m_Registers[regDst] = (m_Ram.GetMemory(m_Registers[regSrcAddr] + 1) << 8) | m_Ram.GetMemory(m_Registers[regSrcAddr]);
}

void CPU::Instruction_LOADB_ADD_TO_REG()
{
    const std::uint16_t imm = m_Prom.Read16(m_ProgramCounter + OpcodeOffset);
    const Register reg = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + ImmediateOffset);

    m_Registers[reg] = m_Ram.GetMemory(imm);
}

void CPU::Instruction_LOADB_REG_TO_REG()
{
    const Register regSrcAddr = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const Register regDst = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + RegisterOffset);

    m_Registers[regDst] = m_Ram.GetMemory(m_Registers[regSrcAddr]);
}

void CPU::Instruction_STORE_REG_TO_ADD()
{
    const Register reg = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const std::uint16_t imm = m_Prom.Read16(m_ProgramCounter + OpcodeOffset + RegisterOffset);

    // We work in little endian
    const std::uint8_t lowerHalf = static_cast<std::uint8_t>(m_Registers[reg]);
    const std::uint8_t upperHalf = static_cast<std::uint8_t>(m_Registers[reg] >> 8);

    m_Ram.SetMemory(imm, lowerHalf);
    m_Ram.SetMemory(imm + 1, upperHalf);
}

void CPU::Instruction_STORE_REG_TO_REG()
{
    const Register regValue = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const Register regDstAddr = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + RegisterOffset);

    // We work in little endian
    const std::uint8_t lowerHalf = static_cast<std::uint8_t>(m_Registers[regValue]);
    const std::uint8_t upperHalf = static_cast<std::uint8_t>(m_Registers[regValue] >> 8);

    m_Ram.SetMemory(m_Registers[regDstAddr], lowerHalf);
    m_Ram.SetMemory(m_Registers[regDstAddr] + 1, upperHalf);
}

void CPU::Instruction_STOREB_REG_TO_ADD()
{
    const Register reg = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const std::uint16_t imm = m_Prom.Read16(m_ProgramCounter + OpcodeOffset + RegisterOffset);

    m_Ram.SetMemory(imm, static_cast<std::uint8_t>(m_Registers[reg]));
}

void CPU::Instruction_STOREB_REG_TO_REG()
{
    const Register regValue = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const Register regDstAddr = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + RegisterOffset);

    m_Ram.SetMemory(m_Registers[regDstAddr], static_cast<std::uint8_t>(m_Registers[regValue]));
}
// ----------------------- Data storage -----------------------


// ----------------------- Program flow -----------------------
void CPU::Instruction_CMP_IMM_TO_REG()
{
    const std::uint16_t a = m_Prom.Read16(m_ProgramCounter + OpcodeOffset);
    const Register reg = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + ImmediateOffset);
    const std::uint16_t b = m_Registers[reg];

    // Virtual subtract: R = b - a
    const std::uint32_t bw = b;
    const std::uint32_t aw = a;
    const std::uint16_t R = static_cast<std::uint16_t>(bw - aw);

    m_Flags.ZF = (R == 0);
    m_Flags.SF = (R >> 15) & 1;
    m_Flags.CF = (bw < aw);
    m_Flags.OF = (((a ^ b) & (b ^ R)) >> 15) & 1;
}

void CPU::Instruction_CMP_REG_TO_REG()
{
    const Register reg1 = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const Register reg2 = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + RegisterOffset);

    const std::uint16_t a = m_Registers[reg1];
    const std::uint16_t b = m_Registers[reg2];
    const std::uint16_t R = static_cast<std::uint16_t>(std::uint32_t(b) - std::uint32_t(a));

    m_Flags.ZF = (R == 0);
    m_Flags.SF = (R >> 15) & 1;
    m_Flags.CF = (uint32_t(b) < uint32_t(a));
    m_Flags.OF = (((a ^ b) & (b ^ R)) >> 15) & 1;
}

void CPU::Instruction_JMP_REG()
{
    const Register reg = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    m_ProgramCounter = m_Registers[reg];
}

void CPU::Instruction_JMP_LABEL()
{
    const std::uint16_t addr = m_Prom.Read16(m_ProgramCounter + OpcodeOffset);
    m_ProgramCounter = addr;
}

void CPU::Instruction_JE_REG(OpcodeMC opcode)
{
    if (m_Flags.ZF)
    {
        const Register reg = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
        m_ProgramCounter = m_Registers[reg];
    }
    else
        m_ProgramCounter += s_InstructionIRSizeMap.at(opcode);
}

void CPU::Instruction_JE_LABEL(OpcodeMC opcode)
{
    if (m_Flags.ZF)
        m_ProgramCounter = m_Prom.Read16(m_ProgramCounter + OpcodeOffset);
    else
        m_ProgramCounter += s_InstructionIRSizeMap.at(opcode);
}

void CPU::Instruction_BRK()
{
    m_ExecutionMode = false; // Stay stuck at this instructions, NOT a bug
}

void CPU::Instruction_HALT()
{
    m_WaitingOnHalt = true;
    while (m_WaitingOnHalt.load(std::memory_order_relaxed));
}
// ----------------------- Program flow -----------------------


// ------------------- Arithmetic operations -------------------
void CPU::Instruction_ADD_IMM_TO_REG()
{
    const std::uint16_t imm = m_Prom.Read16(m_ProgramCounter + OpcodeOffset);
    const Register reg = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + ImmediateOffset);

    m_Registers[reg] += imm;
}

void CPU::Instruction_SUB_IMM_TO_REG()
{
    const std::uint16_t imm = m_Prom.Read16(m_ProgramCounter + OpcodeOffset);
    const Register reg = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + ImmediateOffset);

    m_Registers[reg] -= imm;
}

void CPU::Instruction_ADD_REG_TO_REG()
{
    const Register regSrc = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const Register regDst = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + RegisterOffset);
    m_Registers[regDst] += m_Registers[regSrc];
}

void CPU::Instruction_SUB_REG_TO_REG()
{
    const Register regSrc = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const Register regDst = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + RegisterOffset);
    m_Registers[regDst] -= m_Registers[regSrc];
}

// =============================================================
// MULTIPLICATION (MUL / IMUL)
// Result stored in DX:AX (R2:R0)
// =============================================================
void CPU::Instruction_MUL_IMM_TO_REG()
{
    const std::uint16_t value = m_Prom.Read16(m_ProgramCounter + OpcodeOffset);
    const Register reg = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + ImmediateOffset);

    const std::uint32_t result = (std::uint32_t)m_Registers[reg] * (std::uint32_t)value;
    m_Registers[R1] = (std::uint16_t)(result & 0xFFFF);
    m_Registers[R0] = (std::uint16_t)((result >> 16) & 0xFFFF);
}

void CPU::Instruction_MUL_REG_TO_REG()
{
    const Register reg1 = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const Register reg2 = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + RegisterOffset);

    const std::uint32_t result = (std::uint32_t)m_Registers[reg1] * (std::uint32_t)m_Registers[reg2];
    m_Registers[R1] = (std::uint16_t)(result & 0xFFFF);
    m_Registers[R0] = (std::uint16_t)((result >> 16) & 0xFFFF);
}

void CPU::Instruction_IMUL_IMM_TO_REG()
{
    const std::int16_t value = (std::int16_t)m_Prom.Read16(m_ProgramCounter + OpcodeOffset);
    const Register reg = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + ImmediateOffset);

    // Note: Widening cast to int32_t preserves sign
    const std::int32_t result = (std::int32_t)((std::int16_t)m_Registers[reg]) * (std::int32_t)value;
    m_Registers[R1] = (std::uint16_t)(result & 0xFFFF);
    m_Registers[R0] = (std::uint16_t)((result >> 16) & 0xFFFF);
}

void CPU::Instruction_IMUL_REG_TO_REG()
{
    const Register reg1 = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const Register reg2 = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + RegisterOffset);

    // Note: Widening cast to int32_t preserves sign
    const std::int32_t result = (std::int32_t)((std::int16_t)m_Registers[reg1]) * (std::int32_t)((std::int16_t)m_Registers[reg2]);
    m_Registers[R1] = (std::uint16_t)(result & 0xFFFF);
    m_Registers[R0] = (std::uint16_t)((result >> 16) & 0xFFFF);
}

// =============================================================
// DIVISION (DIV / IDIV)
// Dividend: DX:AX (R2:R0)
// Quotient -> AX (R0), Remainder -> DX (R2)
// =============================================================
void CPU::Instruction_DIV_IMM_TO_REG()
{
    const std::uint16_t value = m_Prom.Read16(m_ProgramCounter + OpcodeOffset);
    const Register reg = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + ImmediateOffset);

    m_Registers[R1] = (std::uint16_t)(m_Registers[reg] / value); // Quotient
    m_Registers[R0] = (std::uint16_t)(m_Registers[reg] % value); // Remainder
}

void CPU::Instruction_DIV_REG_TO_REG()
{
    const Register reg1 = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const Register reg2 = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + RegisterOffset);

    m_Registers[R1] = (std::uint16_t)(m_Registers[reg2] / m_Registers[reg1]);
    m_Registers[R0] = (std::uint16_t)(m_Registers[reg2] % m_Registers[reg1]);
}

void CPU::Instruction_IDIV_IMM_TO_REG()
{
    const std::uint16_t value = m_Prom.Read16(m_ProgramCounter + OpcodeOffset);
    const Register reg = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + ImmediateOffset);

    m_Registers[R1] = ((std::int16_t)m_Registers[reg] / (std::int16_t)value); // Quotient
    m_Registers[R0] = ((std::int16_t)m_Registers[reg] % (std::int16_t)value); // Remainder
}

void CPU::Instruction_IDIV_REG_TO_REG()
{
    const Register reg1 = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const Register reg2 = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + RegisterOffset);

    m_Registers[R1] = ((std::int16_t)m_Registers[reg2] / (std::int16_t)m_Registers[reg1]);
    m_Registers[R0] = ((std::int16_t)m_Registers[reg2] % (std::int16_t)m_Registers[reg1]);
}

void CPU::Instruction_NEG_REG()
{
    const Register dst = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    m_Registers[dst] = -m_Registers[dst];
}
// ------------------- Arithmetic operations -------------------


// ---------------------- Byte operations ----------------------
void CPU::Instruction_EXTBL_REG_TO_REG()
{
    const Register src = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const Register dst = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + RegisterOffset);
    m_Registers[dst] = m_Registers[src] & 0x00FF;
}

void CPU::Instruction_EXTBH_REG_TO_REG()
{
    const Register src = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const Register dst = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + RegisterOffset);
    m_Registers[dst] = (m_Registers[src] >> 8) & 0x00FF;
}

void CPU::Instruction_INSBL_REG_TO_REG()
{
    const Register src = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const Register dst = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + RegisterOffset);
    m_Registers[dst] = (m_Registers[dst] & 0x00FF) | (m_Registers[src] & 0x00FF);
}

void CPU::Instruction_INSBH_REG_TO_REG()
{
    const Register src = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const Register dst = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + RegisterOffset);
    m_Registers[dst] = (m_Registers[dst] & 0x00FF) | ((m_Registers[src] & 0x00FF) << 8);
}

void CPU::Instruction_SWAPB_REG()
{
    const Register dst = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    m_Registers[dst] = ((m_Registers[dst] & 0x00FF) << 8) | (m_Registers[dst] >> 8);
}

void CPU::Instruction_AND_REG_TO_REG()
{
    const Register src = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const Register dst = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + RegisterOffset);
    m_Registers[dst] = m_Registers[src] & m_Registers[dst];
}

void CPU::Instruction_AND_IMM_TO_REG()
{
    const std::uint16_t imm = m_Prom.Read16(m_ProgramCounter + OpcodeOffset);
    const Register dst = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + ImmediateOffset);
    m_Registers[dst] = imm & m_Registers[dst];
}

void CPU::Instruction_OR_REG_TO_REG()
{
    const Register src = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const Register dst = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + RegisterOffset);
    m_Registers[dst] = m_Registers[src] | m_Registers[dst];
}

void CPU::Instruction_OR_IMM_TO_REG()
{
    const std::uint16_t imm = m_Prom.Read16(m_ProgramCounter + OpcodeOffset);
    const Register dst = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + ImmediateOffset);
    m_Registers[dst] = imm | m_Registers[dst];
}

void CPU::Instruction_XOR_REG_TO_REG()
{
    const Register src = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset);
    const Register dst = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + RegisterOffset);
    m_Registers[dst] = m_Registers[src] ^ m_Registers[dst];
}

void CPU::Instruction_XOR_IMM_TO_REG()
{
    const std::uint16_t imm = m_Prom.Read16(m_ProgramCounter + OpcodeOffset);
    const Register dst = (Register)m_Prom.Read(m_ProgramCounter + OpcodeOffset + ImmediateOffset);
    m_Registers[dst] = imm ^ m_Registers[dst];
}
// ---------------------- Byte operations ----------------------



void CPU::Clock()
{
    /*
        Decided to use a massive switch statement due to a 20 - 50 % performance
        increase compared to the lookup table
    */
    const OpcodeMC opcode = (OpcodeMC)m_Prom.Read(m_ProgramCounter);
    switch (opcode)
    {
    case OpcodeIR::MOV_IMM_TO_REG:      Instruction_MOV_IMM_TO_REG();  break;
    case OpcodeIR::ADD_IMM_TO_REG:      Instruction_ADD_IMM_TO_REG();  break;
    case OpcodeIR::MUL_IMM_TO_REG:      Instruction_MUL_IMM_TO_REG();  break;
    case OpcodeIR::IMUL_IMM_TO_REG:     Instruction_IMUL_IMM_TO_REG(); break;
    case OpcodeIR::DIV_IMM_TO_REG:      Instruction_DIV_IMM_TO_REG();  break;
    case OpcodeIR::IDIV_IMM_TO_REG:     Instruction_IDIV_IMM_TO_REG(); break;
    case OpcodeIR::SUB_IMM_TO_REG:      Instruction_SUB_IMM_TO_REG();  break;
    case OpcodeIR::CMP_IMM_TO_REG:      Instruction_CMP_IMM_TO_REG();  break;

    case OpcodeIR::MOV_REG_TO_REG:      Instruction_MOV_REG_TO_REG();  break;
    case OpcodeIR::ADD_REG_TO_REG:      Instruction_ADD_REG_TO_REG();  break;
    case OpcodeIR::MUL_REG_TO_REG:      Instruction_MUL_REG_TO_REG();  break;
    case OpcodeIR::IMUL_REG_TO_REG:     Instruction_IMUL_REG_TO_REG(); break;
    case OpcodeIR::DIV_REG_TO_REG:      Instruction_DIV_REG_TO_REG();  break;
    case OpcodeIR::IDIV_REG_TO_REG:     Instruction_IDIV_REG_TO_REG(); break;
    case OpcodeIR::SUB_REG_TO_REG:      Instruction_SUB_REG_TO_REG();  break;
    case OpcodeIR::CMP_REG_TO_REG:      Instruction_CMP_REG_TO_REG();  break;

    // We return on jump because we don't want to advance the program counter
    // If a jmp has to advance it increments it itself
    case OpcodeIR::JMP_REG:             return Instruction_JMP_REG();
    case OpcodeIR::JMP_LABEL:           return Instruction_JMP_LABEL();
    case OpcodeIR::JE_REG:              return Instruction_JE_REG(opcode);
    case OpcodeIR::JE_LABEL:            return Instruction_JE_LABEL(opcode);

    case OpcodeIR::BRK:                 return Instruction_BRK();
    case OpcodeIR::HALT:                Instruction_HALT(); break;
    case OpcodeIR::NOP:                 break;

    case OpcodeIR::LOAD_ADD_TO_REG:     Instruction_LOAD_ADD_TO_REG();   break;
    case OpcodeIR::LOAD_REG_TO_REG:     Instruction_LOAD_REG_TO_REG();   break;
    case OpcodeIR::LOADB_ADD_TO_REG:    Instruction_LOADB_ADD_TO_REG();  break;
    case OpcodeIR::LOADB_REG_TO_REG:    Instruction_LOADB_REG_TO_REG();  break;
    case OpcodeIR::STORE_REG_TO_ADD:    Instruction_STORE_REG_TO_ADD();  break;
    case OpcodeIR::STORE_REG_TO_REG:    Instruction_STORE_REG_TO_REG();  break;
    case OpcodeIR::STOREB_REG_TO_ADD:   Instruction_STOREB_REG_TO_ADD(); break;
    case OpcodeIR::STOREB_REG_TO_REG:   Instruction_STOREB_REG_TO_REG(); break;


    case OpcodeIR::EXTBH_REG_TO_REG:    Instruction_EXTBH_REG_TO_REG(); break;
    case OpcodeIR::EXTBL_REG_TO_REG:    Instruction_EXTBL_REG_TO_REG(); break;
    case OpcodeIR::INSBH_REG_TO_REG:    Instruction_INSBH_REG_TO_REG(); break;
    case OpcodeIR::INSBL_REG_TO_REG:    Instruction_INSBL_REG_TO_REG(); break;
    case OpcodeIR::SWAPB_REG:           Instruction_SWAPB_REG();        break;
    case OpcodeIR::AND_REG_TO_REG:      Instruction_AND_REG_TO_REG();   break;
    case OpcodeIR::AND_IMM_TO_REG:      Instruction_AND_IMM_TO_REG();   break;
    case OpcodeIR::OR_REG_TO_REG:       Instruction_OR_REG_TO_REG();    break;
    case OpcodeIR::OR_IMM_TO_REG:       Instruction_OR_IMM_TO_REG();    break;
    case OpcodeIR::XOR_REG_TO_REG:      Instruction_XOR_REG_TO_REG();   break;
    case OpcodeIR::XOR_IMM_TO_REG:      Instruction_XOR_IMM_TO_REG();   break;
    case OpcodeIR::NEG_REG:             Instruction_NEG_REG();          break;

    default:
        m_ErrorMsg = "Execution failed: unknown instruction or memory read error. Did you miss a BRK instruction?";
        m_ExecutionMode = false;
        return;
    }
    m_ProgramCounter += s_InstructionIRSizeMap.at(opcode);
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