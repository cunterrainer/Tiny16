#ifndef FILE_HPP
#define FILE_HPP
#include <vector>
#include <cstdint>
#include <string_view>

#include "Result.hpp"

Result<std::vector<std::uint8_t>> LoadFile(std::string_view path);

#endif // FILE_HPP