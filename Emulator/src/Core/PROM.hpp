#ifndef PROM_HPP
#define PROM_HPP

#include <vector>
#include <cstdint>

#include "Assembler/lib/src/Intermediate.hpp"

#include "Log.hpp"

class PROM
{
private:
    std::vector<std::uint8_t> m_SourceCode;
public:
    inline void LoadProgam(const std::vector<std::uint8_t>& machineCode)
    {
        m_SourceCode = machineCode;
    }

    inline std::uint8_t Read(std::uint16_t address) const
    {
        ERR_IF(address >= m_SourceCode.size(), "PROM::Read address greater than machine code size, address: {}, machine code size: {}", address, m_SourceCode.size());
        return m_SourceCode[address];
    }

    // Reads at address and address + 1
    inline std::uint16_t Read16(std::uint16_t address) const
    {
        // Reminder: Little endian architecture
        ERR_IF(address + 1 >= m_SourceCode.size(), "PROM::Read16 address greater than machine code size, address: {}, machine code size: {}", address, m_SourceCode.size());
        return (m_SourceCode[address + 1] << 8) | m_SourceCode[address];
    }
};

#endif // PROM_HPP
