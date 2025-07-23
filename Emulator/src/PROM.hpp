#ifndef PROM_HPP
#define PROM_HPP

#include <vector>
#include <cstdint>

class PROM
{
private:
    std::vector<std::uint8_t> m_SourceCode;
public:
    explicit PROM(const std::vector<std::uint8_t>& code) : m_SourceCode(code) {}

    std::uint8_t Read(std::uint16_t address)
    {
        return m_SourceCode[address];
    }

    // Reads at address and address + 1
    std::uint16_t Read16(std::uint16_t address)
    {
        // Reminder: Little endian architecture
        return (m_SourceCode[address + 1] << 8) | m_SourceCode[address];
    }
};

#endif // PROM_HPP