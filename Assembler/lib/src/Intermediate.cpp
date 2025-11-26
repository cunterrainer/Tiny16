#include <format>
#include <string>
#include <cctype>
#include <cstdint>
#include <optional>
#include <charconv>
#include <stdexcept>
#include <algorithm>
#include <system_error>
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


OperandIR ParseOperand(std::string str)
{
    THROW_IF(str.empty(), std::format("Unreachable code: Failed to parse operand, operand is empty!"));
    
    const auto toUpper = [](unsigned char c) { return std::toupper(c); };
    std::transform(str.begin(), str.end(), str.begin(), toUpper);
    if (IsValidRegister(str))
    {
        return { OperandTypeIR::Register, m_RegisterMap.at(str) };
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
    if (str.size() == 2 && str[0] == 'R')
    {
        const std::uint8_t reg = static_cast<std::uint8_t>(str[1] - '0');
        if (reg <= 7)
            return { OperandTypeIR::Register, reg };
    }

    return { OperandTypeIR::Label, str }; // It's a label
}


OpcodeIR GetOpcodeIR(Opcode opcode, OperandTypeIR op1, OperandTypeIR op2)
{
    const auto it = s_OpcodeIRMapping.find(opcode);
    if (it == s_OpcodeIRMapping.end())
    {
        throw std::logic_error(std::format("Unknown Opcode in GetOpcodeIR: {}", (int)opcode));
    }

    const auto& mapping = it->second;

    if (mapping.relevantOperandIndex == 0)
    {
        return mapping.resIfRegister; // Both are the same for index 0
    }

    OperandTypeIR typeToCheck = (mapping.relevantOperandIndex == 1) ? op1 : op2;
    if (typeToCheck == OperandTypeIR::Register)
    {
        return mapping.resIfRegister;
    }

    return mapping.resIfOther;
}


OperandIR LowerOperand(OperandType operand, const std::string& parsedOperand, [[ maybe_unused ]] const ParsedInstruction& parsedInstr)
{
    OperandIR operandIr;

    switch (operand)
    {
    case OperandType::Register:
    case OperandType::Intermediate:
    case OperandType::RegisterOrIntermediate:
        operandIr = ParseOperand(parsedOperand);
        THROW_IF(operandIr.type == OperandTypeIR::Register && std::get<std::uint8_t>(operandIr.value) > 7, std::format("LowerInstruction: Register '{}' not valid, check validator. Instruction: {} {}, {} Line: {}", parsedInstr.lhs, parsedInstr.opcode, parsedInstr.lhs, parsedInstr.rhs, parsedInstr.lineNumber));
        break;
    case OperandType::Label:
    case OperandType::RegisterOrLabel:
        operandIr = ParseOperandJmp(parsedOperand);
        break;
    case OperandType::None:
        operandIr.type = OperandTypeIR::None;
        THROW_IF(!parsedOperand.empty(), std::format("LowerInstruction: Operand is not empty but type is none: {}, Line: {}", parsedInstr.lhs, parsedInstr.lineNumber));
        break;
    default:
        throw std::logic_error(std::format("LowerInstruction operand {} invalid", parsedOperand));
        break;
    }
    return operandIr;
}


InstructionIR LowerInstruction(const ParsedInstruction& parsedInstr)
{
    const auto instrOpt = LookupOpcode(parsedInstr.opcode);
    THROW_IF(!instrOpt.has_value(), std::format("LowerInstruction: Opcode '{}' not found, check validator. Instruction: {} {}, {} Line: {}", parsedInstr.opcode, parsedInstr.opcode, parsedInstr.lhs, parsedInstr.rhs, parsedInstr.lineNumber));
    const Instruction& instr = instrOpt.value();

    InstructionIR instrIr;
    instrIr.op1 = LowerOperand(instr.op1, parsedInstr.lhs, parsedInstr);
    instrIr.op2 = LowerOperand(instr.op2, parsedInstr.rhs, parsedInstr);
   
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
