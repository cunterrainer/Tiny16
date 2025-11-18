#include <string>
#include <format>
#include <cctype>
#include <optional>
#include <algorithm>
#include <string_view>
#include <unordered_set>

#include "Parser.hpp"
#include "Utility.hpp"
#include "Validator.hpp"
#include "Instruction.hpp"

#include "Utility/Result.hpp"

bool IsValidIntermediate(std::string_view s)
{
    if (s.size() < 2 || s[0] != '$') return false;

    s.remove_prefix(1); // Remove '$'

    // Remember if there's a leading '+' or '-'
    bool isNegative = false;
    if (!s.empty() && (s[0] == '+' || s[0] == '-'))
    {
        isNegative = (s[0] == '-');
        s.remove_prefix(1);
    }

    if (s.starts_with("0x") || s.starts_with("0X"))
    {
        s.remove_prefix(2);
        return !s.empty() && std::all_of(s.begin(), s.end(), [](char c) {
            return std::isxdigit(static_cast<unsigned char>(c));
        });
    }
    else if (s.starts_with("0b") || s.starts_with("0B"))
    {
        if (isNegative)
            return false; // No negative binary allowed

        s.remove_prefix(2);
        return !s.empty() && std::all_of(s.begin(), s.end(), [](char c) {
            return c == '0' || c == '1';
        });
    }
    else
    {
        // Decimal case
        return !s.empty() && std::all_of(s.begin(), s.end(), [](char c) {
            return std::isdigit(static_cast<unsigned char>(c));
        });
    }
}


bool IsValidRegister(const std::string_view s)
{
    // including lower and uppercase R is easier than uppercasing every string
    return s == "R0" || s == "R1" || s == "R2" || s == "R3" ||
        s == "R4" || s == "R5" || s == "R6" || s == "R7" ||
        s == "r0" || s == "r1" || s == "r2" || s == "r3" ||
        s == "r4" || s == "r5" || s == "r6" || s == "r7";
}


bool IsValidLabel(const std::string& label, const std::unordered_set<std::string>& labels)
{
    return labels.contains(label);
}


std::optional<Instruction> LookupOpcode(const std::string& opcode)
{
    const auto& it = s_InstructionMap.find(opcode);
    if (it == s_InstructionMap.end())
    {
        return std::nullopt;
    }
    return it->second;
}


Result<void> ValidateOperand(OperandType operand, const std::string& parsedOperand, const ParsedInstruction& parsedInstr, std::string_view sourceOrDest, const std::unordered_set<std::string>& labels)
{
    if (operand == OperandType::None && !parsedOperand.empty())
    {
        return Err("Instruction: {} {}, {}\nToo many operands", parsedInstr.opcode, parsedInstr.lhs, parsedInstr.rhs);
    }
    else if (operand == OperandType::Register && !IsValidRegister(parsedOperand))
    {
        return Err("Instruction: {} {}, {}\nInvalid {} register: {}", parsedInstr.opcode, parsedInstr.lhs, parsedInstr.rhs, sourceOrDest, parsedOperand);
    }
    else if (operand == OperandType::Intermediate && !IsValidIntermediate(parsedOperand))
    {
        return Err("Instruction: {} {}, {}\nInvalid {} value: {}", parsedInstr.opcode, parsedInstr.lhs, parsedInstr.rhs, sourceOrDest, parsedOperand);
    }
    else if (operand == OperandType::RegisterOrIntermediate && !IsValidIntermediate(parsedOperand) && !IsValidRegister(parsedOperand))
    {
        return Err("Instruction: {} {}, {}\nInvalid {} register or {} value: {}", parsedInstr.opcode, parsedInstr.lhs, parsedInstr.rhs, sourceOrDest, sourceOrDest, parsedOperand);
    }
    else if (operand == OperandType::RegisterOrLabel && !IsValidLabel(parsedOperand, labels) && !IsValidRegister(parsedOperand))
    {
        return Err("Instruction: {} {}, {}\nInvalid register or label: {}", parsedInstr.opcode, parsedInstr.lhs, parsedInstr.rhs, parsedOperand);
    }
    return Ok();
}


Result<void> ValidateInstruction(const ParsedInstruction& parsedInstr, const std::unordered_set<std::string>& labels)
{
    const auto toUpperCase = [](unsigned char c) { return std::toupper(c); };

    std::string opcode = parsedInstr.opcode;
    std::transform(opcode.begin(), opcode.end(), opcode.begin(), toUpperCase);

    const std::optional<Instruction> info = LookupOpcode(opcode);

    if (!info.has_value())
    {
        return Err("Unknown instruction: {}\nLine: {}, {} {} {}", parsedInstr.opcode, parsedInstr.lineNumber, parsedInstr.opcode, parsedInstr.lhs, parsedInstr.rhs);
    }

    const Instruction instrInfo = info.value();
    const Result<void> op1Result = ValidateOperand(instrInfo.op1, parsedInstr.lhs, parsedInstr, "source", labels);
    const Result<void> op2Result = ValidateOperand(instrInfo.op2, parsedInstr.rhs, parsedInstr, "destination", labels);

    if (op1Result.IsErr()) return op1Result;
    if (op2Result.IsErr()) return op2Result;
    return Ok();
}