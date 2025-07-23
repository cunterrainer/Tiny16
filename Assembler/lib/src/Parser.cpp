#include <regex>
#include <vector>
#include <string>
#include <format>
#include <iostream>
#include <optional>

#include "Parser.hpp"
#include "Utility.hpp"


std::optional<Instruction> ParseLine(std::string line, size_t lineNumber)
{
    // Strip comments
    if (const size_t commentPos = line.find('#'); commentPos != std::string::npos)
        line = line.substr(0, commentPos);

    line = Trim(line);
    if (line.empty()) return std::nullopt;
   
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

    Instruction instr;
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

    std::cerr << std::format("Error: Could not parse line '{}', Line {}", line, lineNumber) << std::endl;
    return std::nullopt;
}


// TODO Assign an Opcode a label
std::vector<Instruction> ParseSourceCode(const std::vector<std::string>& lines)
{
    std::vector<Instruction> instructions;

    for (size_t i = 0; i < lines.size(); i++)
    {
        std::optional<Instruction> instr = ParseLine(lines[i], i);
        std::cout << lines[i] << std::endl;

        if (!instr.has_value())
            continue;
        instructions.push_back(instr.value());
#ifndef NDEBUG
        std::cout << "Label: " << instr.value().label
            << " | Opcode: " << instr.value().opcode
            << " | Op1: " << instr.value().lhs
            << " | Op2: " << instr.value().rhs << "\n";
#endif
    }

    return instructions;
}