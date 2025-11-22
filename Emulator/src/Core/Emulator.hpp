#ifndef CORE_EMULATOR_HPP
#define CORE_EMULATOR_HPP

#include <atomic>
#include <thread>
#include <memory>
#include <chrono>
#include <vector>
#include <cstdint>

#include "CPU.hpp"
#include "RAM.hpp"
#include "PROM.hpp"

namespace Core
{
    class Emulator
    {
    public:
        std::unique_ptr<CPU> m_CPU = std::make_unique<CPU>(m_PROM, m_RAM);
        RAM m_RAM;
        PROM m_PROM;

        std::atomic<double> m_Frequenz = 0;
        double m_Elapsed = 0;
        bool m_ProgramLoaded = false;
        std::thread m_CpuThread;
        std::atomic<bool> m_IsExecuting = false;
    public:
        inline ~Emulator()
        {
            StopExecution();
        }


        inline void Step() noexcept
        {
            if (!m_ProgramLoaded || m_IsExecuting || !m_CPU->IsExecuting()) return;
            m_CPU->Clock();
        }


        inline void StopExecution() noexcept
        {
            if (!m_IsExecuting) return;
            m_IsExecuting = false;
            m_CpuThread.join();
        }


        void StartExecution() noexcept;
        void LoadProgram(const std::vector<std::uint8_t>& machineCode);
    };
}

#endif // CORE_EMULATOR_HPP