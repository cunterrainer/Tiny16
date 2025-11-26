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
        RBP
    };

    struct Flags
    {
        std::uint32_t ZF = 0;
        std::uint32_t SF = 0;
        std::uint32_t CF = 0;
        std::uint32_t OF = 0;
    };
private:
    const PROM& m_Prom;
    RAM& m_Ram;
    Flags m_Flags;

    std::string m_ErrorMsg;
    bool m_ExecutionMode = true;
    std::atomic_bool m_WaitingOnHalt = false;
    std::uint16_t m_ProgramCounter = 0;
    std::array<std::uint16_t, static_cast<std::size_t>(Register::RBP) + 1> m_Registers = { 0 };
private:
    // Instruction functions
    // Data storage
    void Instruction_MOV_IMM_TO_REG   ();
    void Instruction_MOV_REG_TO_REG   ();
    void Instruction_LOAD_ADD_TO_REG  ();
    void Instruction_LOAD_REG_TO_REG  ();
    void Instruction_LOADB_ADD_TO_REG ();
    void Instruction_LOADB_REG_TO_REG ();
    void Instruction_STORE_REG_TO_ADD ();
    void Instruction_STORE_REG_TO_REG ();
    void Instruction_STOREB_REG_TO_ADD();
    void Instruction_STOREB_REG_TO_REG();

    // Program flow
    void Instruction_CMP_REG_TO_REG   ();
    void Instruction_CMP_IMM_TO_REG   ();
    void Instruction_JMP_REG          ();
    void Instruction_JMP_LABEL        ();
    void Instruction_JE_REG           (OpcodeMC opcode);
    void Instruction_JE_LABEL         (OpcodeMC opcode);
    void Instruction_JNE_LABEL        (OpcodeMC opcode);
    void Instruction_JG_LABEL         (OpcodeMC opcode);
    void Instruction_JGE_LABEL        (OpcodeMC opcode);
    void Instruction_JL_LABEL         (OpcodeMC opcode);
    void Instruction_JLE_LABEL        (OpcodeMC opcode);
    void Instruction_JA_LABEL         (OpcodeMC opcode);
    void Instruction_JAE_LABEL        (OpcodeMC opcode);
    void Instruction_JB_LABEL         (OpcodeMC opcode);
    void Instruction_JBE_LABEL        (OpcodeMC opcode);
    void Instruction_HALT             ();
    void Instruction_BRK              ();

    // Arithmetic operations
    void Instruction_ADD_IMM_TO_REG   ();
    void Instruction_SUB_IMM_TO_REG   ();
    void Instruction_ADD_REG_TO_REG   ();
    void Instruction_SUB_REG_TO_REG   ();
    void Instruction_MUL_IMM_TO_REG   ();
    void Instruction_MUL_REG_TO_REG   ();
    void Instruction_IMUL_IMM_TO_REG  ();
    void Instruction_IMUL_REG_TO_REG  ();
    void Instruction_DIV_IMM_TO_REG   ();
    void Instruction_DIV_REG_TO_REG   ();
    void Instruction_IDIV_IMM_TO_REG  ();
    void Instruction_IDIV_REG_TO_REG  ();
    void Instruction_NEG_REG          ();
    
    // Byte operations
    void Instruction_EXTBH_REG_TO_REG ();
    void Instruction_EXTBL_REG_TO_REG ();
    void Instruction_INSBH_REG_TO_REG ();
    void Instruction_INSBL_REG_TO_REG ();
    void Instruction_SWAPB_REG        ();
    void Instruction_AND_REG_TO_REG   ();
    void Instruction_AND_IMM_TO_REG   ();
    void Instruction_OR_REG_TO_REG    ();
    void Instruction_OR_IMM_TO_REG    ();
    void Instruction_XOR_REG_TO_REG   ();
    void Instruction_XOR_IMM_TO_REG   ();
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