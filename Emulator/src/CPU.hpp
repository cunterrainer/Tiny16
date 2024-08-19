#ifndef CPU_H
#define CPU_H
#include <array>
#include <vector>
#include <cstdint>

#ifndef NDEBUG
#define CPU_PRINT_REGISTERS(cpu) cpu.Debug_PrintRegisters()
#else
#define CPU_PRINT_REGISTERS(cpu)
#endif

class CPU
{
public:
    enum class Instruction
    {
        MOVI  = 20,
        MOVR  = 21,
        ADDI  = 30,
        ADDR  = 31,
        SUBI  = 32,
        SUBR  = 33,
        MULI  = 34,
        MULR  = 35,
        IMULI = 36,
        IMULR = 37,
        DIVI  = 38,
        DIVR  = 39,
        IDIVI = 40,
        IDIVR = 41,
        EXIT = 0xFF
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
        RS,
        RB,
        RF  // Reserved for flags can't be used
    };

private:
    std::array<std::uint16_t, static_cast<std::size_t>(Register::RF) + 1> m_Registers = { 0 };
private:
    inline std::uint16_t GetImmediate16(const std::uint8_t* ptr) const noexcept;
public:
    void Execute(const std::vector<std::uint8_t>& code) noexcept;

    #ifndef NDEBUG
        void Debug_PrintRegisters() const;
    #endif
};

#endif // CPU_H