#include <vector>
#include <string>
#include <cctype>
#include <format>
#include <ranges>
#include <utility>
#include <sstream>
#include <cstring>
#include <iostream>
#include <optional>
#include <algorithm>

#include "Parser.hpp"
#include "Utility.hpp"

std::vector<std::string> Tokenize(const std::string& line)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream stream(line);

    while (stream >> token)
    {
        if (token.ends_with(',')) // e.g. MOV $4, R0 | $4,
        {
            token.pop_back();
            tokens.push_back(token);
        }
        else if (const size_t commaPos = token.find(','); commaPos != std::string::npos) // e.g. MOV $4,R0 | $4,R0
        {
            tokens.push_back(token.substr(0, commaPos));
            tokens.push_back(token.substr(commaPos + 1));
        }
        else // e.g. HLT | HLT
        {
            tokens.push_back(token);
        }
    }
    return tokens;
}


std::optional<Instruction> ParseLine(std::string line, size_t lineNumber)
{
    // Strip comment
    const size_t commentPos = line.find('#');
    if (commentPos != std::string::npos)
        line = line.substr(0, commentPos);

    line = Trim(line);
    if (line.empty())
        return std::nullopt;

    std::vector<std::string> tokens = Tokenize(line);
    if (tokens.empty()) return std::nullopt;

    Instruction instr;
    instr.lineNumber = lineNumber;

    if (tokens.begin()->ends_with(':'))
    {
        instr.label = tokens.begin()->substr(0, tokens.begin()->size() - 1);

        if (tokens.size() == 1)
        {
            return instr;
        }
        else
        {
            std::cerr << std::format("Error: token found after Label: '{}', Line {}", instr.label, instr.lineNumber) << std::endl;
            return std::nullopt;
        }
    }

    instr.opcode = tokens[0];

    if (tokens.size() > 1)
        instr.lhs = tokens[1];
    if (tokens.size() > 2)
        instr.rhs = tokens[2];

    return instr;
}


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