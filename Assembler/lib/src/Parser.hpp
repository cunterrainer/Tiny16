#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <optional>

#include "Utility/Result.hpp"

struct ParsedInstruction
{
    size_t lineNumber = 0;
    std::string label;
    std::string opcode;
    std::string lhs;
    std::string rhs;
};

Result<ParsedInstruction> ParseLine(std::string line, size_t lineNumber);
Result<std::vector<ParsedInstruction>> ParseSourceCode(const std::vector<std::string>& lines);

#endif // PARSER_H