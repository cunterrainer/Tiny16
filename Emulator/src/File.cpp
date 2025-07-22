#include <vector>
#include <cstdint>
#include <fstream>
#include <optional>
#include <iterator>
#include <iostream>
#include <string_view>

#include "Log.hpp"
#include "File.hpp"

std::optional<std::vector<std::uint8_t>> LoadFile(std::string_view path)
{
    std::ifstream file(path.data(), std::ios::in | std::ios::binary);

    if (!file.is_open())
    {
        LOG_REASON("Failed to open file: '{}'", path);
        return {};
    }

    return std::vector<uint8_t>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}