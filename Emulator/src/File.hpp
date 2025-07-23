#ifndef FILE_HPP
#define FILE_HPP
#include <vector>
#include <cstdint>
#include <optional>
#include <string_view>

std::optional<std::vector<std::uint8_t>> LoadFile(std::string_view path);

#endif // FILE_HPP