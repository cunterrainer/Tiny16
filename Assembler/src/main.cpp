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


void ParseSourceCode(const std::vector<std::string>& lines)
{
    for (auto line : lines)
    {
        line = Trim(line);
        if (line.empty()) continue;

        // Lowercase in-place
        std::ranges::transform(line, line.begin(), [](unsigned char c) { return std::tolower(c); });

        // Comment
        if (line[0] == ';') continue;



        std::cout << line << std::endl;
    }
}


int main()
{
    std::optional<std::vector<std::string>> sourceLines = ReadFile("examples/example1.s");
    if (!sourceLines)
        return 1;

    ParseSourceCode(sourceLines.value());

    std::cin.get();
    return 0;
}