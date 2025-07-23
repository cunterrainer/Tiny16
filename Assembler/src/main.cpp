#include <vector>
#include <string>
#include <cerrno>
#include <cctype>
#include <ranges>
#include <utility>
#include <sstream>
#include <fstream>
#include <cstring>
#include <iostream>
#include <optional>
#include <algorithm>
#include <filesystem>
#include <system_error>

std::optional<std::vector<std::string>> ReadFile(const std::filesystem::path& path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        const char* const reason = std::strerror(errno);
        std::cout << std::format("Failed to open file '{}', Reason: {}\n", path.string(), reason) << std::endl;
        return std::nullopt;
    }

    std::vector<std::string> lines;

    std::string line;
    while (std::getline(file, line))
    {
        lines.emplace_back(std::move(line));
    }

    return lines;
}


std::string Trim(std::string_view str)
{
    // 1. Trim start
    auto is_not_space = [](unsigned char c) { return !std::isspace(c); };
    auto start = std::ranges::find_if(str, is_not_space);

    // 2. Trim end
    auto end = std::ranges::find_if(str | std::views::reverse, is_not_space).base();

    if (start >= end) return std::string(); // all spaces

    // 3. Create trimmed string
    return std::string(start, end);
}

struct Instruction
{
    size_t lineNumber;
    std::string label;
    std::string opcode;
    std::string lhs;
    std::string rhs;
};



std::vector<std::string> Tokenize(const std::string& line)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream stream(line);

    while (stream >> token)
    {
        if (token.ends_with(','))
        {
            token.pop_back();
            tokens.push_back(token);
        }
        else
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
    if (commentPos != std::string_view::npos)
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
        // Lowercase in-place
        std::string line = lines[i];
        std::ranges::transform(line, line.begin(), [](unsigned char c) { return std::tolower(c); });

        std::optional<Instruction> instr = ParseLine(line, i);
        std::cout << line << std::endl;

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


int main()
{
    std::optional<std::vector<std::string>> sourceLines = ReadFile("examples/example2.s");
    if (!sourceLines)
        return 1;

    std::vector<Instruction> instructions = ParseSourceCode(sourceLines.value());
    
    std::cin.get();
    return 0;
}