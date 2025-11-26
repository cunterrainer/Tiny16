#ifndef CPU_H
#define CPU_H
#include <array>
#include <atomic>
#include <vector>
#include <string>
#include <cstdint>
#include <unordered_map>

#include "../../Assembler/lib/src/Assembler.hpp"

#include "RAM.hpp"
#include "PROM.hpp"

class CPU
{
private:
    constexpr static std::uint16_t ImmediateOffset = 2;
    constexpr static std::uint16_t OpcodeOffset    = 1;
    constexpr static std::uint16_t RegisterOffset  = 1;
public:
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
        RA,
        RB,
        RC,
        RD,
        RE,
        RSP,
        RBP,
        RF  // Reserved for flags can't be used
    };

    enum Flags
    {
        // TODO implement carry and borrow
        Less    = 0b00000001,
        Equal   = 0b00000010,
        Greater = 0b00000100,
        Carry   = 0b00001000,
        Borrow  = 0b00010000
    };
private:
    const PROM& m_Prom;
    RAM& m_Ram;
    std::string m_ErrorMsg;
    bool m_ExecutionMode = true;
    std::atomic_bool m_WaitingOnHalt = false;
    std::uint16_t m_ProgramCounter = 0;
    std::array<std::uint16_t, static_cast<std::size_t>(Register::RF) + 1> m_Registers = { 0 };
private:
    // Instruction functions
    // Data storage
    void Instruction_MOV_IMM_TO_REG   (OpcodeMC opcode);
    void Instruction_MOV_REG_TO_REG   (OpcodeMC opcode);
    void Instruction_LOAD_ADD_TO_REG  (OpcodeMC opcode);
    void Instruction_LOAD_REG_TO_REG  (OpcodeMC opcode);
    void Instruction_LOADB_ADD_TO_REG (OpcodeMC opcode);
    void Instruction_LOADB_REG_TO_REG (OpcodeMC opcode);
    void Instruction_STORE_REG_TO_ADD (OpcodeMC opcode);
    void Instruction_STORE_REG_TO_REG (OpcodeMC opcode);
    void Instruction_STOREB_REG_TO_ADD(OpcodeMC opcode);
    void Instruction_STOREB_REG_TO_REG(OpcodeMC opcode);

    // Program flow
    void Instruction_CMP_REG_TO_REG   (OpcodeMC opcode);
    void Instruction_CMP_IMM_TO_REG   (OpcodeMC opcode);
    void Instruction_JMP_REG          (OpcodeMC opcode);
    void Instruction_JMP_LABEL        (OpcodeMC opcode);
    void Instruction_JE_REG           (OpcodeMC opcode);
    void Instruction_JE_LABEL         (OpcodeMC opcode);
    void Instruction_HALT             (OpcodeMC opcode);
    void Instruction_BRK              (OpcodeMC opcode);
    void Instruction_NOP              (OpcodeMC opcode);

    // Arithmetic operations
    void Instruction_ADD_IMM_TO_REG   (OpcodeMC opcode);
    void Instruction_SUB_IMM_TO_REG   (OpcodeMC opcode);
    void Instruction_ADD_REG_TO_REG   (OpcodeMC opcode);
    void Instruction_SUB_REG_TO_REG   (OpcodeMC opcode);
    void Instruction_NEG_REG          (OpcodeMC opcode);
    
    // Byte operations
    void Instruction_EXTBH_REG_TO_REG (OpcodeMC opcode);
    void Instruction_EXTBL_REG_TO_REG (OpcodeMC opcode);
    void Instruction_INSBH_REG_TO_REG (OpcodeMC opcode);
    void Instruction_INSBL_REG_TO_REG (OpcodeMC opcode);
    void Instruction_SWAPB_REG        (OpcodeMC opcode);
    void Instruction_AND_REG_TO_REG   (OpcodeMC opcode);
    void Instruction_AND_IMM_TO_REG   (OpcodeMC opcode);
    void Instruction_OR_REG_TO_REG    (OpcodeMC opcode);
    void Instruction_OR_IMM_TO_REG    (OpcodeMC opcode);
    void Instruction_XOR_REG_TO_REG   (OpcodeMC opcode);
    void Instruction_XOR_IMM_TO_REG   (OpcodeMC opcode);
public:
    explicit CPU(const PROM& prom, RAM& ram) : m_Prom(prom), m_Ram(ram) {};
    CPU(CPU&& cpu) noexcept;
    CPU(const CPU& cpu);
    CPU& operator=(CPU&& cpu) = delete;
    CPU& operator=(const CPU& cpu) = delete;

    void Clock();

    inline void FireVBlankInterrupt() noexcept { m_WaitingOnHalt = false; }
    constexpr bool IsExecuting() const noexcept { return m_ExecutionMode; }
    constexpr std::uint16_t GetRegister(Register reg) const noexcept { return m_Registers[reg]; }
    constexpr std::uint16_t GetProgramCounter() const noexcept { return m_ProgramCounter; }
    const std::string& GetErrorMsg() const noexcept { return m_ErrorMsg; }
};

#endif // CPU_H