#include <vector>
#include <string>
#include <cerrno>
#include <cctype>
#include <ranges>
#include <format>
#include <utility>
#include <sstream>
#include <fstream>
#include <cstring>
#include <iostream>
#include <optional>
#include <algorithm>
#include <string_view>
#include <system_error>

#include "Utility.hpp"

std::optional<std::vector<std::string>> ReadFile(const std::string& path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        const char* const reason = std::strerror(errno);
        std::cout << std::format("Failed to open file '{}', Reason: {}\n", path, reason) << std::endl;
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