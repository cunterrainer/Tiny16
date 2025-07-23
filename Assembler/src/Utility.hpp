#ifndef UTILITY_HPP
#define UTILITY_HPP
#include <bit>
#include <cstdint>

namespace Util::Bytes
{
    consteval bool HostIsBigEndian() noexcept
    {
        return std::endian::native == std::endian::big;
    }


    constexpr std::uint16_t SwapEndian16(std::uint16_t value) noexcept
    {
        return (value >> 8) | (value << 8);
    }
}

#endif // UTILITY_HPP