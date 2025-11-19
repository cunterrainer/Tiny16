#ifndef RAM_HPP
#define RAM_HPP

#include <vector>
#include <limits>
#include <cstdint>

class RAM
{
private:
    std::vector<std::uint16_t> m_Memory = std::vector<std::uint16_t>(std::numeric_limits<std::uint16_t>::max() + 1 /* +1 for the 65536 byte */, 0);
public:
    inline std::uint16_t GetMemory(std::uint16_t address) const noexcept
    {
        return m_Memory[address];
    }

    inline void SetMemory(std::uint16_t address, std::uint16_t value) noexcept
    {
        m_Memory[address] = value;
    }

    inline size_t GetSize() const noexcept
    {
        return m_Memory.size();
    }
};


#endif // RAM_HPP