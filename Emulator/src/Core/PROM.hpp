#ifndef PROM_HPP
#define PROM_HPP

#include <vector>
#include <cstdint>

class PROM
{
private:
    std::vector<std::uint8_t> m_SourceCode;
public:
    inline void LoadProgam(const std::vector<std::uint8_t>& machineCode)
    {
        m_SourceCode = machineCode;
    }

    inline std::uint8_t Read(std::uint16_t address) const noexcept
    {
        return m_SourceCode[address];
    }

    // Reads at address and address + 1
    inline std::uint16_t Read16(std::uint16_t address) const noexcept
    {
        // Reminder: Little endian architecture
        return (m_SourceCode[address + 1] << 8) | m_SourceCode[address];
    }
};

#endif // PROM_HPP