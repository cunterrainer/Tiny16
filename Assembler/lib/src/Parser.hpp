#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <utility>
#include <optional>
#include <unordered_set>

#include "Error.hpp"

#include "Utility/Result.hpp"

struct ParsedInstruction
{
    size_t lineNumber = 0;
    std::string label;
    std::string opcode;
    std::string lhs;
    std::string rhs;
};

Result<ParsedInstruction, ASMError> ParseLine(std::string line, size_t lineNumber);
Result<std::pair<std::vector<ParsedInstruction>, std::unordered_set<std::string>>, ASMError> ParseSourceCode(const std::vector<std::string>& lines);

#endif // PARSER_H