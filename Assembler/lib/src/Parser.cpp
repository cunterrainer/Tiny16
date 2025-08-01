#include <regex>
#include <vector>
#include <string>
#include <format>
#include <utility>
#include <iostream>
#include <optional>
#include <unordered_set>

#include "Parser.hpp"
#include "Utility.hpp"

#include "Utility/Result.hpp"


Result<ParsedInstruction> ParseLine(std::string line, size_t lineNumber)
{
    // Strip comments
    if (const size_t commentPos = line.find('#'); commentPos != std::string::npos)
        line = line.substr(0, commentPos);

    line = Trim(line);
    if (line.empty()) return ParsedInstruction();
   
    // Matches assembly instructions with 0, 1, or 2 operands in the form:
    //     OPCODE
    //     OPCODE OPERAND
    //     OPCODE OPERAND , OPERAND
    // Spaces are allowed freely around operands and comma, but the comma must be present for 2 operands.
    //
    // Regex breakdown:
    // ^\s*                      → Optional leading whitespace
    // ([A-Za-z]+)              → Group 1: The instruction mnemonic (e.g., MOV, ADD)
    // (?:                      → Begin optional operand group (non-capturing)
    //     \s+([^,\s]+)         → Group 2: First operand (non-comma, non-space sequence), preceded by at least one space
    //     (?:\s*,\s*([^,\s]+))?→ Optional Group 3: Second operand after comma, allowing spaces around the comma
    // )?                       → End optional operand group
    // \s*$                     → Optional trailing whitespace until end of line
    static const std::regex instrRegex(R"(^\s*([A-Za-z]+)(?:\s+([^,\s]+)(?:\s*,\s*([^,\s]+))?)?\s*$)");
    static const std::regex labelRegex(R"(^\s*([A-Za-z_][\w]*):\s*$)"); // Needs from Label: first letter can be A-Za-z or _

    std::smatch match;

    ParsedInstruction instr;
    instr.lineNumber = lineNumber;

    if (std::regex_match(line, match, labelRegex))
    {
        instr.label = match[1].str();
        instr.opcode = "";
        instr.lhs = "";
        instr.rhs = "";
        return instr;
    }

    if (std::regex_match(line, match, instrRegex))
    {
        instr.label = "";
        instr.opcode = match[1].str();
        if (match[2].matched) instr.lhs = match[2].str();
        if (match[3].matched) instr.rhs = match[3].str();
        return instr;
    }

    return Err("Error: Failed to parse line {}: '{}', unknown label or instruction structure", lineNumber, line);
}


Result<std::pair<std::vector<ParsedInstruction>, std::unordered_set<std::string>>> ParseSourceCode(const std::vector<std::string>& lines)
{
    std::vector<ParsedInstruction> instructions;
    std::unordered_set<std::string> labels;

    for (size_t i = 0; i < lines.size(); i++)
    {
        Result<ParsedInstruction> instr = ParseLine(lines[i], i);

        if (instr.IsErr())
        {
            return instr.Err();
        }

        if (instr.Ok().label.empty() && instr.Ok().opcode.empty()) // Was a comment or empty line
        {
            continue;
        }

        if (!instructions.empty() && !instructions.back().label.empty() && instructions.back().opcode.empty())
        {
            const ParsedInstruction& tmp = instr.Ok();
            ParsedInstruction& a = instructions.back();
            a.opcode = tmp.opcode;
            a.lineNumber = tmp.lineNumber;
            a.lhs = tmp.lhs;
            a.rhs = tmp.rhs;

            const auto res = labels.insert(a.label);
            if (!res.second)
                return Err("Line: {}, Label: '{}' already exists", a.lineNumber, a.label);
        }
        else
        {
            instructions.emplace_back(std::move(instr.OkTake()));
        }
//#ifndef NDEBUG
//        std::cout << "Label: " << instructions.back().label
//            << " | Opcode: " << instructions.back().opcode
//            << " | Op1: " << instructions.back().lhs
//            << " | Op2: " << instructions.back().rhs << "\n";
//#endif
    }

    return Ok<std::pair<std::vector<ParsedInstruction>, std::unordered_set<std::string>>>(instructions, labels);
}