#ifndef CPU_H
#define CPU_H
#include <array>
#include <vector>
#include <string>
#include <cstdint>

#include "PROM.hpp"

class CPU
{
public:
    enum class Instruction
    {
        MOVI = 20,
        MOVR = 21,
        ADDI = 30,
        ADDR = 31,
        SUBI = 32,
        SUBR = 33,
        JMP  = 50,
        JE   = 51,
        CMPI = 60,
        CMPR = 61,
        HLT  = 0xFF
    };

    enum Register
    {
        R0,
        R1,
        R2,
        R3,
        R4,
        R5,
        R6,
        R7,
        R8,
        R9,
        RF  // Reserved for flags can't be used
    };

    enum Flags
    {
        Less    = 0b00000001,
        Equal   = 0b00000010,
        Greater = 0b00000100,
        Carry   = 0b00001000,
        Borrow  = 0b00010000
    };
private:
    PROM& m_Prom;
    std::uint16_t m_ProgramCounter = 0;
    std::array<std::uint16_t, static_cast<std::size_t>(Register::RF) + 1> m_Registers = { 0 };
private:
    inline std::uint16_t GetImmediate16(const std::uint8_t* ptr) const noexcept;
public:
    explicit CPU(PROM& prom) : m_Prom(prom)
    {
    };

    std::string Clock() noexcept;
    constexpr std::uint16_t GetRegister(Register reg) const noexcept { return m_Registers[reg]; }
    constexpr std::uint16_t GetProgramCounter() const noexcept { return m_ProgramCounter; }
    
    void MOVI();
    void MOVR();
    void ADDI();
    void ADDR();
    void SUBI();
    void SUBR();
    void JMP();
    void JE();
    void CMPI();
    void CMPR();
    void HLT();
};

#endif // CPU_H