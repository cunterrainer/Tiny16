#include <format>
#include <string>
#include <cstdint>
#include <optional>
#include <charconv>
#include <stdexcept>
#include <string_view>

#include "Parser.hpp"
#include "Intermediate.hpp"

OpcodeIR ToOpcode(const std::string_view str)
{
    if (str == "MOV")   return OpcodeIR::MOV;
    if (str == "ADD")   return OpcodeIR::ADD;
    if (str == "SUB")   return OpcodeIR::SUB;
    if (str == "CMP")   return OpcodeIR::CMP;
    if (str == "JMP")   return OpcodeIR::JMP;
    if (str == "JE")    return OpcodeIR::JE;
    if (str == "HLT")   return OpcodeIR::HLT;
    if (str == "LOAD")  return OpcodeIR::LOAD;
    if (str == "STORE") return OpcodeIR::STORE;
    
    throw std::logic_error(std::format("Unreachable code, check for errors in opcode validation code.\nOpcode: {}", str));
}


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

    // Parse into a temporary unsigned value (safest)
    int value = 0;
    auto result = std::from_chars(immStr.data(), immStr.data() + immStr.size(), value, base);
    if (result.ec != std::errc())
    {
        throw std::logic_error("Invalid immediate value: " + operand + " check validation code");
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


std::optional<Operand> ParseOperand(const std::string& str)
{
    if (str.empty()) return std::nullopt;

    if (str[0] == 'R' || str[0] == 'r')
    {
        const int reg = std::stoi(str.substr(1));
        return { { OperandType::Register, static_cast<uint16_t>(reg) } };
    }

    if (str[0] == '$')
        {
        uint16_t val = ParseIntermediate(str);
        return { { OperandType::Immediate, val } };
    }
    
    return std::nullopt;
}


InstructionIR LowerInstruction(const ParsedInstruction& parsedInstr)
{
    const std::optional<Operand> op1 = ParseOperand(parsedInstr.lhs);
    if (!op1)
    {
        throw std::logic_error(std::format("Unreachable code: Failed to parse operand1, check validation code\nOpcode: {}, Operand1: {}, Operand2: {}", parsedInstr.opcode, parsedInstr.lhs, parsedInstr.rhs));
    }
    
    const std::optional<Operand> op2 = ParseOperand(parsedInstr.rhs);
    if (!op2)
    {
        throw std::logic_error(std::format("Unreachable code: Failed to parse operand2, check validation code\nOpcode: {}, Operand1: {}, Operand2: {}", parsedInstr.opcode, parsedInstr.lhs, parsedInstr.rhs));
    }
    
    InstructionIR instruction;
    instruction.label = parsedInstr.label;
    instruction.opcode = ToOpcode(parsedInstr.opcode);
    instruction.op1 = op1.value();
    instruction.op2 = op2.value();
    return instruction;
}