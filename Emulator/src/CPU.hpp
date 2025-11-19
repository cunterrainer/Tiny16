#ifndef CPU_H
#define CPU_H
#include <array>
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
    bool m_ExecutionMode = true;
    std::uint16_t m_ProgramCounter = 0;
    std::array<std::uint16_t, static_cast<std::size_t>(Register::RF) + 1> m_Registers = { 0 };

    using InstructionFunction = void (CPU::*) (OpcodeMC);
    const std::unordered_map<OpcodeMC, InstructionFunction> m_InstructionFunctionTable = {
        { OpcodeIR::MOV_IMM_TO_REG, &CPU::Instruction_MOV_IMM_TO_REG },
        { OpcodeIR::ADD_IMM_TO_REG, &CPU::Instruction_ADD_IMM_TO_REG },
        { OpcodeIR::SUB_IMM_TO_REG, &CPU::Instruction_SUB_IMM_TO_REG },
        { OpcodeIR::CMP_IMM_TO_REG, &CPU::Instruction_CMP_IMM_TO_REG },
        { OpcodeIR::MOV_REG_TO_REG, &CPU::Instruction_MOV_REG_TO_REG },
        { OpcodeIR::ADD_REG_TO_REG, &CPU::Instruction_ADD_REG_TO_REG },
        { OpcodeIR::SUB_REG_TO_REG, &CPU::Instruction_SUB_REG_TO_REG },
        { OpcodeIR::CMP_REG_TO_REG, &CPU::Instruction_CMP_REG_TO_REG },
        { OpcodeIR::JMP_REG       , &CPU::Instruction_JMP_REG        },
        { OpcodeIR::JMP_LABEL     , &CPU::Instruction_JMP_LABEL      },
        { OpcodeIR::JE_REG        , &CPU::Instruction_JE_REG         },
        { OpcodeIR::JE_LABEL      , &CPU::Instruction_JE_LABEL       },
        { OpcodeIR::HLT           , &CPU::Instruction_HLT            },
        { OpcodeIR::LOAD          , &CPU::Instruction_LOAD           },
        { OpcodeIR::STORE         , &CPU::Instruction_STORE          },
    };
private:
    inline std::uint16_t GetImmediate16(const std::uint8_t* ptr) const noexcept;

    // Instruction functions
    void Instruction_MOV_IMM_TO_REG(OpcodeMC opcode);
    void Instruction_ADD_IMM_TO_REG(OpcodeMC opcode);
    void Instruction_SUB_IMM_TO_REG(OpcodeMC opcode);
    void Instruction_CMP_IMM_TO_REG(OpcodeMC opcode);
    void Instruction_MOV_REG_TO_REG(OpcodeMC opcode);
    void Instruction_ADD_REG_TO_REG(OpcodeMC opcode);
    void Instruction_SUB_REG_TO_REG(OpcodeMC opcode);
    void Instruction_CMP_REG_TO_REG(OpcodeMC opcode);
    void Instruction_JMP_REG       (OpcodeMC opcode);
    void Instruction_JMP_LABEL     (OpcodeMC opcode);
    void Instruction_JE_REG        (OpcodeMC opcode);
    void Instruction_JE_LABEL      (OpcodeMC opcode);
    void Instruction_HLT           (OpcodeMC opcode);
    void Instruction_LOAD          (OpcodeMC opcode);
    void Instruction_STORE         (OpcodeMC opcode);
public:
    explicit CPU(const PROM& prom, RAM& ram) : m_Prom(prom), m_Ram(ram) {};
    void Clock();

    constexpr bool IsExecuting() const noexcept { return m_ExecutionMode; }
    constexpr std::uint16_t GetRegister(Register reg) const noexcept { return m_Registers[reg]; }
    constexpr std::uint16_t GetProgramCounter() const noexcept { return m_ProgramCounter; }
};

#endif // CPU_H