#include <string>
#include <format>
#include <cctype>
#include <algorithm>
#include <string_view>

#include "Parser.hpp"

struct ValidationResult
{
    bool valid;
    std::string errorMsg;
};


bool IsValidImmediate(std::string_view s)
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
            return false; // ❌ No negative binary allowed

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



bool IsValidRegister(std::string_view s)
{
    return s == "R0" || s == "R1" || s == "R2" || s == "R3" ||
        s == "R4" || s == "R5" || s == "R6" || s == "R7";
}


ValidationResult ValidateInstruction(const Instruction& instr)
{
    const auto toUpperCase = [](unsigned char c) { return std::toupper(c); };

    std::string opcode = instr.opcode;
    std::transform(opcode.begin(), opcode.end(), opcode.begin(), toUpperCase);

    if (opcode == "HLT")
    {
        if (!instr.lhs.empty())
        {
            return { false, std::format("Instruction: {} {}, {}\nToo many operands, correct form: {}", instr.opcode, instr.lhs, instr.rhs, instr.opcode) };
        }
    }
    // TODO Check if label exists
    else if (opcode == "JMP" || opcode == "JE")
    {
        if (instr.lhs.empty())
        {
            return { false, std::format("Instruction: {}\nNot enough operands, correct form: {} Label", instr.opcode, instr.opcode) };
        }
        if (!instr.lhs.empty() && !instr.rhs.empty())
        {
            return { false, std::format("Instruction: {} {}, {}\nToo many operands, correct form: {} Label", instr.opcode, instr.lhs, instr.rhs, instr.opcode) };
        }
    }
    else if (opcode == "MOV" || opcode == "ADD" || opcode == "SUB" || opcode == "CMP")
    {
        if (instr.lhs.empty() || instr.rhs.empty())
        {
            return { false, std::format("Instruction: {} {}\nNot enought operands, correct form: {} {}, Register", instr.opcode, instr.lhs, instr.opcode, instr.lhs) };
        }
        if (!IsValidRegister(instr.rhs))
        {
            return { false, std::format("Instruction: {} {}, {}\nInvalid destination register: {}", instr.opcode, instr.lhs, instr.rhs, instr.rhs) };
        }
        if (!IsValidImmediate(instr.lhs) && !IsValidRegister(instr.lhs))
        {
            return { false, std::format("Instruction: {} {}, {}\nInvalid source register or source intermediate value: {}", instr.opcode, instr.lhs, instr.rhs, instr.lhs) };
        }
    }
    else
    {
        return { false, std::format("Instruction {} {} {}\nUnknown instruction", instr.opcode, instr.lhs, instr.rhs) };
    }

    return { true, "" };
}


ValidationResult ValidateInstructions(const std::vector<Instruction>& instructions, std::string_view filePath)
{
    for (const Instruction& i : instructions)
    {
        ValidationResult result = ValidateInstruction(i);
        if (!result.valid)
        {
            result.errorMsg = std::format("{}\nLine: {}, File: {}", result.errorMsg, i.lineNumber, filePath);
        }
    }

    return { };
}