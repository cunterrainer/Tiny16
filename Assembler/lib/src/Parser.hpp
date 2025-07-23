#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <optional>

struct Instruction
{
    size_t lineNumber;
    std::string label;
    std::string opcode;
    std::string lhs;
    std::string rhs;
};

std::vector<std::string> Tokenize(const std::string& line);
std::optional<Instruction> ParseLine(std::string line, size_t lineNumber);
std::vector<Instruction> ParseSourceCode(const std::vector<std::string>& lines);

#endif // PARSER_H