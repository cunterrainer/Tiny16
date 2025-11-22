#include <atomic>
#include <thread>
#include <memory>
#include <chrono>
#include <vector>
#include <cstdint>

#include "Core/Emulator.hpp"

namespace Core
{
    void Emulator::StartExecution() noexcept
    {
        if (!m_ProgramLoaded || m_IsExecuting || !m_CPU->IsExecuting()) return;

        m_IsExecuting = true;
        m_CpuThread = std::thread(
            [&]() {
                int count = 0;
                auto start = std::chrono::steady_clock::now();
                while (m_CPU->IsExecuting() && m_IsExecuting)
                {
                    m_CPU->Clock();
                    ++count;

                    if (count == 1000000)
                    {
                        auto end = std::chrono::steady_clock::now();
                        std::chrono::duration<double> elapsed = end - start;
                        m_Frequenz = count / elapsed.count();
                        m_Elapsed = elapsed.count();
                        count = 0;
                        start = std::chrono::steady_clock::now();
                    }
                }
            });
    }


    void Emulator::LoadProgram(const std::vector<std::uint8_t>& machineCode)
    {
        StopExecution();
        m_PROM.LoadProgam(machineCode);

        // We don't have to reset the ram because, as a developer, you can't just assume that a ram cell has a specific
        // value without seting it first. Thus it's expected behaviour to have garbage values in ram

        m_CPU = std::make_unique<CPU>(m_PROM, m_RAM);
        m_ProgramLoaded = true;
    }
}