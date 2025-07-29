#ifndef UTILITY_H
#define UTILITY_H

#include <vector>
#include <string>
#include <optional>
#include <string_view>

#include "Utility/Result.hpp"

std::optional<std::vector<std::string>> ReadFile(const std::string& path);
std::string Trim(std::string_view str);

#endif // UTILITY_H