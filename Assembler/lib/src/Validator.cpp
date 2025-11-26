#include <string>
#include <format>
#include <cctype>
#include <optional>
#include <algorithm>
#include <string_view>
#include <unordered_set>

#include "Error.hpp"
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


bool IsValidRegister(std::string s)
{
    const auto toUpperCase = [](unsigned char c) { return std::toupper(c); };
    std::transform(s.begin(), s.end(), s.begin(), toUpperCase);

    for (auto const& [key, val] : m_RegisterMap)
    {
        if (s == key)
            return true;
    }
    return false;
}


bool IsValidLabel(const std::string& label, const std::unordered_set<std::string>& labels)
{
    return labels.contains(label);
}


std::optional<Instruction> LookupOpcode(std::string opcode)
{
    const auto toUpper = [](unsigned char c) { return std::toupper(c); };
    std::transform(opcode.begin(), opcode.end(), opcode.begin(), toUpper);

    const auto& it = s_InstructionMap.find(opcode);
    if (it == s_InstructionMap.end())
    {
        return std::nullopt;
    }
    return it->second;
}


Result<void, ASMError> ValidateOperand(OperandType operand, const std::string& parsedOperand, const ParsedInstruction& parsedInstr, std::string_view sourceOrDest, const std::unordered_set<std::string>& labels)
{
    if (operand == OperandType::None && !parsedOperand.empty())
    {
        return ASMError(parsedInstr.lineNumber, "Instruction: {} {}, {}\nToo many operands", parsedInstr.opcode, parsedInstr.lhs, parsedInstr.rhs);
    }
    else if (operand == OperandType::Register && !IsValidRegister(parsedOperand))
    {
        return ASMError(parsedInstr.lineNumber, "Instruction: {} {}, {}\nInvalid {} register: {}", parsedInstr.opcode, parsedInstr.lhs, parsedInstr.rhs, sourceOrDest, parsedOperand);
    }
    else if (operand == OperandType::Intermediate && !IsValidIntermediate(parsedOperand))
    {
        return ASMError(parsedInstr.lineNumber, "Instruction: {} {}, {}\nInvalid {} value: {}", parsedInstr.opcode, parsedInstr.lhs, parsedInstr.rhs, sourceOrDest, parsedOperand);
    }
    else if (operand == OperandType::RegisterOrIntermediate && !IsValidIntermediate(parsedOperand) && !IsValidRegister(parsedOperand))
    {
        return ASMError(parsedInstr.lineNumber, "Instruction: {} {}, {}\nInvalid {} register or {} value: {}", parsedInstr.opcode, parsedInstr.lhs, parsedInstr.rhs, sourceOrDest, sourceOrDest, parsedOperand);
    }
    else if (operand == OperandType::RegisterOrLabel && !IsValidLabel(parsedOperand, labels) && !IsValidRegister(parsedOperand))
    {
        return ASMError(parsedInstr.lineNumber, "Instruction: {} {}, {}\nInvalid register or label: {}", parsedInstr.opcode, parsedInstr.lhs, parsedInstr.rhs, parsedOperand);
    }
    return Ok<ASMError>();
}


Result<void, ASMError> ValidateInstruction(const ParsedInstruction& parsedInstr, const std::unordered_set<std::string>& labels)
{
    const std::optional<Instruction> info = LookupOpcode(parsedInstr.opcode);

    if (!info.has_value())
    {
        return ASMError(parsedInstr.lineNumber, "Unknown instruction: {} in {} {} {}", parsedInstr.opcode, parsedInstr.lineNumber, parsedInstr.opcode, parsedInstr.lhs, parsedInstr.rhs);
    }

    const Instruction instrInfo = info.value();
    const Result<void, ASMError> op1Result = ValidateOperand(instrInfo.op1, parsedInstr.lhs, parsedInstr, "source", labels);
    const Result<void, ASMError> op2Result = ValidateOperand(instrInfo.op2, parsedInstr.rhs, parsedInstr, "destination", labels);

    if (op1Result.IsErr()) return op1Result;
    if (op2Result.IsErr()) return op2Result;
    return Ok<ASMError>();
}


Result<void, ASMError> ValidateAllInstructions(const std::pair<std::vector<ParsedInstruction>, std::unordered_set<std::string>>& parseResult)
{
    const std::vector<ParsedInstruction> parsedInstructions = parseResult.first;
    const std::unordered_set<std::string> labels = parseResult.second;

    for (const auto& instr : parsedInstructions)
    {
        const Result<void, ASMError> result = ValidateInstruction(instr, labels);
        if (result.IsErr())
            return result;
    }
    return Ok<ASMError>();
}