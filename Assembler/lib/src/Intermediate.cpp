#include <format>
#include <string>
#include <cctype>
#include <cstdint>
#include <optional>
#include <charconv>
#include <stdexcept>
#include <algorithm>
#include <string_view>

#include "Parser.hpp"
#include "Validator.hpp"
#include "Instruction.hpp"
#include "Intermediate.hpp"

#ifndef NDEBUG
#define THROW_IF(cond, msg) if (cond) { throw std::logic_error(msg); }
#else
#define THROW_IF(cond, msg)
#endif


std::uint16_t ParseIntermediate(const std::string& operand)
{
    if (operand.empty() || operand[0] != '$')
    {
        throw std::logic_error(std::format("ParseIntermediate: Intermediate has to start with $ check validation code.\nValue: {}", operand));
    }

    std::string immStr = operand.substr(1); // remove '$'

    // Extract optional sign
    int sign = 1;
    if (!immStr.empty() && (immStr[0] == '+' || immStr[0] == '-'))
    {
        if (immStr[0] == '-') sign = -1;
        immStr = immStr.substr(1);
    }

    // Determine base
    int base = 10;
    if (immStr.size() > 1 && (immStr.starts_with("0x") || immStr.starts_with("0X")))
    {
        base = 16;
        immStr = immStr.substr(2);
    }
    else if (immStr.size() > 1 && (immStr.starts_with("0b") || immStr.starts_with("0B")))
    {
        base = 2;
        immStr = immStr.substr(2);
    }
    
    if (base == 2 && sign == -1)
    {
        throw std::logic_error("Invalid binary number sign, check validation code");
    }

    // Parse into a temporary unsigned value (safest)
    int value = 0;
    auto result = std::from_chars(immStr.data(), immStr.data() + immStr.size(), value, base);
    if (result.ec != std::errc())
    {
        throw std::logic_error("Invalid intermediate value: " + operand + " check validation code");
    }

    // Apply sign
    int final = sign * value;

    constexpr std::int16_t min = std::numeric_limits<std::int16_t>::min();  // -32768
    constexpr std::uint16_t max = std::numeric_limits<std::uint16_t>::max();  // +32767

    if (final < min)
    {
        final = min;
    }
    if (final > max)
    {
        final = max;
    }

    // Cast to uint16_t for binary representation (e.g., -4 becomes 0xFFFC)
    return static_cast<uint16_t>(final);
}


OperandIR ParseOperand(const std::string& str)
{
    THROW_IF(str.empty(), std::format("Unreachable code: Failed to parse operand, operand is empty!"));
    
    if (str[0] == 'R' || str[0] == 'r')
    {
        return { OperandTypeIR::Register, static_cast<std::uint8_t>(str[1] - '0')};
    }

    if (str[0] == '$')
    {
        const std::uint16_t val = ParseIntermediate(str);
        return { OperandTypeIR::Intermediate, val };
    }
    
    throw std::logic_error(std::format("Unreachable code: Failed to parse operand, check validation code Operand: {}", str));
}


OperandIR ParseOperandJmp(const std::string& str)
{
    if (str.size() == 2 && (str[0] == 'R' || str[0] == 'r'))
    {
        const std::uint8_t reg = static_cast<std::uint8_t>(str[1] - '0');
        if (reg <= 7)
            return { OperandTypeIR::Register, reg };
    }

    return { OperandTypeIR::Label, str }; // It's a label
}


OpcodeIR GetOpcodeIR(Opcode opcode, OperandTypeIR op1, OperandTypeIR op2)
{
    if (opcode == Opcode::MOV)
    {
        if (op1 == OperandTypeIR::Register)
            return OpcodeIR::MOV_REG_TO_REG;
        return OpcodeIR::MOV_IMM_TO_REG;
    }

    if (opcode == Opcode::ADD)
    {
        if (op1 == OperandTypeIR::Register)
            return OpcodeIR::ADD_REG_TO_REG;
        return OpcodeIR::ADD_IMM_TO_REG;
    }

    if (opcode == Opcode::SUB)
    {
        if (op1 == OperandTypeIR::Register)
            return OpcodeIR::SUB_REG_TO_REG;
        return OpcodeIR::SUB_IMM_TO_REG;
    }

    if (opcode == Opcode::CMP)
    {
        if (op1 == OperandTypeIR::Register)
            return OpcodeIR::CMP_REG_TO_REG;
        return OpcodeIR::CMP_IMM_TO_REG;
    }

    if (opcode == Opcode::HLT)
    {
        return OpcodeIR::HLT;
    }

    if (opcode == Opcode::JMP)
    {
        if (op1 == OperandTypeIR::Register)
            return OpcodeIR::JMP_REG;
        return OpcodeIR::JMP_LABEL;
    }

    if (opcode == Opcode::JE)
    {
        if (op1 == OperandTypeIR::Register)
            return OpcodeIR::JE_REG;
        return OpcodeIR::JE_LABEL;
    }

    if (opcode == Opcode::LOAD)
    {
        if (op1 == OperandTypeIR::Register)
            return OpcodeIR::LOAD_REG_TO_REG;
        return OpcodeIR::LOAD_ADD_TO_REG;
    }

    if (opcode == Opcode::STORE)
    {
        if (op2 == OperandTypeIR::Register)
            return OpcodeIR::STORE_REG_TO_REG;
        return OpcodeIR::STORE_REG_TO_ADD;
    }

    throw std::logic_error(std::format("Unreachable code: GetOpcodeIR, check validation code Operand: {}", (int)opcode));
}


InstructionIR LowerInstruction(const ParsedInstruction& parsedInstr)
{
    std::string opcode = parsedInstr.opcode;
    std::transform(opcode.begin(), opcode.end(), opcode.begin(), [](unsigned char c) {
        return std::toupper(c);
        });

    THROW_IF(!LookupOpcode(opcode).has_value(), std::format("LowerInstruction: Opcode '{}' not found, check validator. Instruction: {} {}, {} Line: {}", parsedInstr.opcode, parsedInstr.opcode, parsedInstr.lhs, parsedInstr.rhs, parsedInstr.lineNumber));
    const Instruction& instr = s_InstructionMap.at(opcode);

    InstructionIR instrIr;
    switch (instr.op1)
    {
    case OperandType::Register:
    case OperandType::Intermediate:
    case OperandType::RegisterOrIntermediate:
        instrIr.op1 = ParseOperand(parsedInstr.lhs);
        THROW_IF(instrIr.op1.type == OperandTypeIR::Register && std::get<std::uint8_t>(instrIr.op1.value) > 7, std::format("LowerInstruction: Register '{}' not valid, check validator. Instruction: {} {}, {} Line: {}", parsedInstr.lhs, parsedInstr.opcode, parsedInstr.lhs, parsedInstr.rhs, parsedInstr.lineNumber));
        break;
    case OperandType::Label:
    case OperandType::RegisterOrLabel:
        instrIr.op1 = ParseOperandJmp(parsedInstr.lhs);
        break;
    case OperandType::None:
        instrIr.op1.type = OperandTypeIR::None;
        THROW_IF(!parsedInstr.lhs.empty(), std::format("LowerInstruction: Operand is not empty but type is none: {}, Line: {}", parsedInstr.lhs, parsedInstr.lineNumber));
        break;
    }

    switch (instr.op2)
    {
    case OperandType::Register:
    case OperandType::Intermediate:
    case OperandType::RegisterOrIntermediate:
        instrIr.op2 = ParseOperand(parsedInstr.rhs);
        THROW_IF(instrIr.op2.type == OperandTypeIR::Register && std::get<std::uint8_t>(instrIr.op2.value) > 7, std::format("LowerInstruction: Register '{}' not valid, check validator. Instruction: {} {}, {} Line: {}", parsedInstr.rhs, parsedInstr.opcode, parsedInstr.lhs, parsedInstr.rhs, parsedInstr.lineNumber));
        break;
    case OperandType::Label:
    case OperandType::RegisterOrLabel:
        instrIr.op2 = ParseOperandJmp(parsedInstr.rhs);
        break;
    case OperandType::None:
        instrIr.op2.type = OperandTypeIR::None;
        THROW_IF(!parsedInstr.rhs.empty(), std::format("LowerInstruction: Operand is not empty but type is none: {}, Line: {}", parsedInstr.rhs, parsedInstr.lineNumber));
        break;
    }

    instrIr.opcode = GetOpcodeIR(instr.opcode, instrIr.op1.type, instrIr.op2.type);
    instrIr.label = parsedInstr.label;
    instrIr.size = s_InstructionIRSizeMap.at(instrIr.opcode);
    return instrIr;
}


std::vector<InstructionIR> LowerAllInstructions(const std::vector<ParsedInstruction>& parsedInstructions)
{
    std::vector<InstructionIR> intermediateInstructions;
    intermediateInstructions.reserve(parsedInstructions.size());

    for (const auto& instr : parsedInstructions)
    {
        intermediateInstructions.push_back(LowerInstruction(instr));
    }
    return intermediateInstructions;
}