#include <vector>
#include <cstdint>

#include "CPU.hpp"
#include "Log.hpp"

#ifndef NDEBUG
#include <iostream>
#include <iomanip>

void CPU::Debug_PrintRegisters() const
{
    std::cout << "Reg   u16    i16\n\n";
    std::cout << "R0: " << std::setw(5) << m_Registers[Register::R0] << ' ' << std::setw(6) << (std::int16_t)m_Registers[Register::R0] << '\n';
    std::cout << "R1: " << std::setw(5) << m_Registers[Register::R1] << ' ' << std::setw(6) << (std::int16_t)m_Registers[Register::R1] << '\n';
    std::cout << "R2: " << std::setw(5) << m_Registers[Register::R2] << ' ' << std::setw(6) << (std::int16_t)m_Registers[Register::R2] << '\n';
    std::cout << "R3: " << std::setw(5) << m_Registers[Register::R3] << ' ' << std::setw(6) << (std::int16_t)m_Registers[Register::R3] << '\n';
    std::cout << "R4: " << std::setw(5) << m_Registers[Register::R4] << ' ' << std::setw(6) << (std::int16_t)m_Registers[Register::R4] << '\n';
    std::cout << "R5: " << std::setw(5) << m_Registers[Register::R5] << ' ' << std::setw(6) << (std::int16_t)m_Registers[Register::R5] << '\n';
    std::cout << "R6: " << std::setw(5) << m_Registers[Register::R6] << ' ' << std::setw(6) << (std::int16_t)m_Registers[Register::R6] << '\n';
    std::cout << "R7: " << std::setw(5) << m_Registers[Register::R7] << ' ' << std::setw(6) << (std::int16_t)m_Registers[Register::R7] << '\n';
    std::cout << "R8: " << std::setw(5) << m_Registers[Register::R8] << ' ' << std::setw(6) << (std::int16_t)m_Registers[Register::R8] << '\n';
    std::cout << "RS: " << std::setw(5) << m_Registers[Register::RS] << ' ' << std::setw(6) << (std::int16_t)m_Registers[Register::RS] << '\n';
    std::cout << "RB: " << std::setw(5) << m_Registers[Register::RB] << ' ' << std::setw(6) << (std::int16_t)m_Registers[Register::RB] << '\n';
    std::cout << "RF: " << std::setw(5) << m_Registers[Register::RF] << ' ' << std::setw(6) << (std::int16_t)m_Registers[Register::RF] << '\n';
    std::cout << std::endl;
}
#endif


void CPU::Execute(const std::vector<std::uint8_t>& code)
{
    for (std::size_t i = 0; i < code.size();)
    {
        switch (static_cast<Instruction>(code[i]))
        {
        // TODO: Support big endian
        case Instruction::MOVI: // mov (16bit) reg
        {
            const std::uint16_t* imm16 = reinterpret_cast<const std::uint16_t*>(&code[i + 1]);
            const Register reg = static_cast<Register>(code[i + 3]);
            ERR_IF(reg >= Register::RF, "MOVI: Illegal register used: 0x{:X}", static_cast<std::size_t>(reg));
            m_Registers[reg] = *imm16;
            i += 4;
            break;
        }
        case Instruction::MOVR: // mov reg reg
        {
            const std::uint8_t src = code[i + 1];
            const std::uint8_t dest = code[i + 2];
            ERR_IF(src >= Register::RF, "MOVR: Source register doesn't exist: 0x{:X}", src);
            ERR_IF(dest >= Register::RF, "MOVR: Destination register doesn't exist: 0x{:X}", dest);
            m_Registers[dest] = m_Registers[src];
            i += 3;
            break;
        }
        case Instruction::ADDI: // add (16bit) reg
        {
            const std::uint16_t* imm16 = reinterpret_cast<const std::uint16_t*>(&code[i + 1]);
            const Register reg = static_cast<Register>(code[i + 3]);
            ERR_IF(reg >= Register::RF, "ADDI: Illegal register used: 0x{:X}", static_cast<std::size_t>(reg));
            m_Registers[reg] += *imm16;
            i += 4;
            break;
        }
        case Instruction::ADDR: // add reg reg
        {
            const std::uint8_t src = code[i + 1];
            const std::uint8_t dest = code[i + 2];
            ERR_IF(src >= Register::RF, "ADDR: Source register doesn't exist: 0x{:X}", src);
            ERR_IF(dest >= Register::RF, "ADDR: Destination register doesn't exist: 0x{:X}", dest);
            m_Registers[dest] += m_Registers[src];
            i += 3;
            break;
        }
        case Instruction::EXIT:
        {
            return;
        }
        default:
            ERR("Unsupported instruction used: 0x{:X} ({})", static_cast<std::size_t>(code[i]), static_cast<std::size_t>(code[i]));
            break;
        }
    }
}