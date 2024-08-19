#include <vector>
#include <cstdint>

#include "CPU.hpp"
#include "Log.hpp"
#include "Utility.hpp"

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


void CPU::Execute(const std::vector<std::uint8_t>& code) noexcept
{
    for (std::size_t i = 0; i < code.size();)
    {
        // TODO add flags e.g. overflow to add
        switch (static_cast<Instruction>(code[i]))
        {
        case Instruction::MOVI: // mov (16bit) reg
        {
            ERR_IF(i + 4 > code.size(), "MOVI: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 4, code.size() - i, i);

            const std::uint16_t imm = GetImmediate16(&code[i + 1]);
            const Register reg = static_cast<Register>(code[i + 3]);
            ERR_IF(reg >= Register::RF, "MOVI: Illegal register used: 0x{:X}", static_cast<std::size_t>(reg));
            m_Registers[reg] = imm;
            i += 4;
            break;
        }
        case Instruction::MOVR: // mov reg reg
        {
            ERR_IF(i + 3 > code.size(), "MOVR: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 3, code.size() - i, i);

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
            ERR_IF(i + 4 > code.size(), "ADDI: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 4, code.size() - i, i);

            const std::uint16_t imm = GetImmediate16(&code[i + 1]);
            const Register reg = static_cast<Register>(code[i + 3]);
            ERR_IF(reg >= Register::RF, "ADDI: Illegal register used: 0x{:X}", static_cast<std::size_t>(reg));
            m_Registers[reg] += imm;
            i += 4;
            break;
        }
        case Instruction::ADDR: // add reg reg
        {
            ERR_IF(i + 3 > code.size(), "ADDR: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 3, code.size() - i, i);

            const std::uint8_t src = code[i + 1];
            const std::uint8_t dest = code[i + 2];
            ERR_IF(src >= Register::RF, "ADDR: Source register doesn't exist: 0x{:X}", src);
            ERR_IF(dest >= Register::RF, "ADDR: Destination register doesn't exist: 0x{:X}", dest);
            m_Registers[dest] += m_Registers[src];
            i += 3;
            break;
        }
        case Instruction::SUBI: // sub (16bit) reg
        {
            ERR_IF(i + 4 > code.size(), "SUBI: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 4, code.size() - i, i);

            const std::uint16_t imm = GetImmediate16(&code[i + 1]);
            const Register reg = static_cast<Register>(code[i + 3]);
            ERR_IF(reg >= Register::RF, "SUBI: Illegal register used: 0x{:X}", static_cast<std::size_t>(reg));
            m_Registers[reg] -= imm;
            i += 4;
            break;
        }
        case Instruction::SUBR: // sub reg reg
        {
            ERR_IF(i + 3 > code.size(), "SUBR: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 3, code.size() - i, i);

            const std::uint8_t src = code[i + 1];
            const std::uint8_t dest = code[i + 2];
            ERR_IF(src >= Register::RF, "SUBR: Source register doesn't exist: 0x{:X}", src);
            ERR_IF(dest >= Register::RF, "SUBR: Destination register doesn't exist: 0x{:X}", dest);
            m_Registers[dest] -= m_Registers[src];
            i += 3;
            break;
        }
        case Instruction::MULI: // mul (16bit) reg
        {
            ERR_IF(i + 4 > code.size(), "MULI: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 4, code.size() - i, i);

            const std::uint16_t imm = GetImmediate16(&code[i + 1]);
            const Register reg = static_cast<Register>(code[i + 3]);
            ERR_IF(reg >= Register::RF, "MULI: Illegal register used: 0x{:X}", static_cast<std::size_t>(reg));
            m_Registers[reg] *= imm;
            i += 4;
            break;
        }
        case Instruction::MULR: // mul reg reg
        {
            ERR_IF(i + 3 > code.size(), "MULR: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 3, code.size() - i, i);

            const std::uint8_t src = code[i + 1];
            const std::uint8_t dest = code[i + 2];
            ERR_IF(src >= Register::RF, "MULR: Source register doesn't exist: 0x{:X}", src);
            ERR_IF(dest >= Register::RF, "MULR: Destination register doesn't exist: 0x{:X}", dest);
            m_Registers[dest] *= m_Registers[src];
            i += 3;
            break;
        }
        case Instruction::IMULI: // imul (16bit) reg
        {
            ERR_IF(i + 4 > code.size(), "IMULI: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 4, code.size() - i, i);

            const std::uint16_t imm = GetImmediate16(&code[i + 1]);
            const Register reg = static_cast<Register>(code[i + 3]);
            ERR_IF(reg >= Register::RF, "IMULI: Illegal register used: 0x{:X}", static_cast<std::size_t>(reg));
            m_Registers[reg] = static_cast<std::int16_t>(m_Registers[reg]) * static_cast<std::int16_t>(imm);
            i += 4;
            break;
        }
        case Instruction::IMULR: // imul reg reg
        {
            ERR_IF(i + 3 > code.size(), "IMULR: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 3, code.size() - i, i);

            const std::uint8_t src = code[i + 1];
            const std::uint8_t dest = code[i + 2];
            ERR_IF(src >= Register::RF, "IMULR: Source register doesn't exist: 0x{:X}", src);
            ERR_IF(dest >= Register::RF, "IMULR: Destination register doesn't exist: 0x{:X}", dest);
            m_Registers[dest] = static_cast<std::int16_t>(m_Registers[dest]) * static_cast<std::int16_t>(m_Registers[src]);
            i += 3;
            break;
        }
        case Instruction::DIVI: // div (16bit) reg
        {
            ERR_IF(i + 4 > code.size(), "DIVI: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 4, code.size() - i, i);

            const std::uint16_t imm = GetImmediate16(&code[i + 1]);
            const Register reg = static_cast<Register>(code[i + 3]);
            ERR_IF(reg >= Register::RF, "DIVI: Illegal register used: 0x{:X}", static_cast<std::size_t>(reg));

            [[likely]] if (imm != 0)
            {
                // otherwise we may override R0 for the second division
                const uint16_t r0tmp = m_Registers[reg] / imm;
                const uint16_t r1tmp = m_Registers[reg] % imm;
                m_Registers[Register::R0] = r0tmp;
                m_Registers[Register::R1] = r1tmp;
            }
            i += 4;
            break;
        }
        case Instruction::DIVR: // div reg reg
        {
            ERR_IF(i + 3 > code.size(), "DIVR: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 3, code.size() - i, i);

            const std::uint8_t src = code[i + 1];
            const std::uint8_t dest = code[i + 2];
            ERR_IF(src >= Register::RF, "DIVR: Source register doesn't exist: 0x{:X}", src);
            ERR_IF(dest >= Register::RF, "DIVR: Destination register doesn't exist: 0x{:X}", dest);

            [[likely]] if (m_Registers[src] != 0)
            {
                // otherwise the my override R0 for the second division
                const std::uint16_t r0tmp = m_Registers[dest] / m_Registers[src];
                const std::uint16_t r1tmp = m_Registers[dest] % m_Registers[src];
                m_Registers[Register::R0] = r0tmp;
                m_Registers[Register::R1] = r1tmp;
            }
            i += 3;
            break;
        }
        case Instruction::IDIVI: // idiv (16bit) reg
        {
            ERR_IF(i + 4 > code.size(), "IDIVI: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 4, code.size() - i, i);

            const std::uint16_t imm = GetImmediate16(&code[i + 1]);
            const Register reg = static_cast<Register>(code[i + 3]);
            ERR_IF(reg >= Register::RF, "IDIVI: Illegal register used: 0x{:X}", static_cast<std::size_t>(reg));
            m_Registers[reg] = static_cast<std::int16_t>(m_Registers[reg]) * static_cast<std::int16_t>(imm);

            [[likely]] if (imm != 0)
            {
                // otherwise we may override R0 for the second division
                const std::int16_t r0tmp = static_cast<std::int16_t>(m_Registers[reg]) / static_cast<std::int16_t>(imm);
                const std::int16_t r1tmp = static_cast<std::int16_t>(m_Registers[reg]) % static_cast<std::int16_t>(imm);
                m_Registers[Register::R0] = r0tmp;
                m_Registers[Register::R1] = r1tmp;
            }

            i += 4;
            break;
        }
        case Instruction::IDIVR: // idiv reg reg
        {
            ERR_IF(i + 3 > code.size(), "IDIVR: Instruction not complete, expected {} bytes, received {} bytes, code index {}", 3, code.size() - i, i);

            const std::uint8_t src = code[i + 1];
            const std::uint8_t dest = code[i + 2];
            ERR_IF(src >= Register::RF, "IDIVR: Source register doesn't exist: 0x{:X}", src);
            ERR_IF(dest >= Register::RF, "IDIVR: Destination register doesn't exist: 0x{:X}", dest);

            [[likely]] if (m_Registers[src] != 0)
            {
                // otherwise we may override R0 for the second division
                const std::int16_t r0tmp = static_cast<std::int16_t>(m_Registers[dest]) / static_cast<std::int16_t>(m_Registers[src]);
                const std::int16_t r1tmp = static_cast<std::int16_t>(m_Registers[dest]) % static_cast<std::int16_t>(m_Registers[src]);
                m_Registers[Register::R0] = r0tmp;
                m_Registers[Register::R1] = r1tmp;
            }

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