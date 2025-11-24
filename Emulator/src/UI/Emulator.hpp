#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <cstdio>
#include <memory>
#include <vector>
#include <cstdint>
#include <algorithm>

#include "imgui.h"

#include "UI/Screen.hpp"
#include "Core/CPU.hpp"
#include "Core/RAM.hpp"
#include "Core/PROM.hpp"
#include "Core/Emulator.hpp"
#include "Disassembler/Disassembler.hpp"

namespace UI
{
    class Emulator
    {
    private:
        Disassembler m_Disassembler;

        Core::Emulator m_Emulator;
        bool m_ShouldReset = false;
        Screen m_Screen;
    public:
        Emulator() : m_Screen(m_Emulator.GetScreenPixel(UI::Screen::Width, UI::Screen::Height)) {}

        void LoadProgram(const std::vector<std::uint8_t>& machineCode)
        {
            m_Emulator.LoadProgram(machineCode);
            m_Disassembler.Disassemble(machineCode);
        }

        inline bool ShouldReset() const noexcept
        {
            return m_ShouldReset;
        }

        inline void SetShouldReset(bool value) noexcept
        {
            m_ShouldReset = value;
        }

        inline void StartExecution() noexcept
        {
            m_Emulator.StartExecution();
        }

        void Render()
        {
            Show();
        }

        void Show()
        {
            const auto instrWidth = 350;
            const auto width = ImGui::GetWindowWidth() - instrWidth - ImGui::GetStyle().WindowPadding.x * 2;
            float cursorY = 0;

            ImGui::BeginChild(1, { width, 85 }, ImGuiChildFlags_Borders);
            {
                cursorY = ImGui::GetWindowPos().y;
                float item_width = 100;

                ImGui::Text("%f", m_Emulator.m_Frequenz.load());
                ImGui::Text("%f", m_Emulator.m_Elapsed);
                ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##R0Label", "R0: 0x%04X", m_Emulator.m_CPU->GetRegister(CPU::Register::R0));
                ImGui::SameLine(); ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##R3Label", "R3: 0x%04X", m_Emulator.m_CPU->GetRegister(CPU::Register::R3));
                ImGui::SameLine(); ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##R6Label", "R6: 0x%04X", m_Emulator.m_CPU->GetRegister(CPU::Register::R6));
                ImGui::SameLine(); ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##PCLabel", "PC: 0x%04X", m_Emulator.m_CPU->GetProgramCounter());
                
                ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##R1Label", "R1: 0x%04X", m_Emulator.m_CPU->GetRegister(CPU::Register::R1));
                ImGui::SameLine(); ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##R4Label", "R4: 0x%04X", m_Emulator.m_CPU->GetRegister(CPU::Register::R4));
                ImGui::SameLine(); ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##R7Label", "R7: 0x%04X", m_Emulator.m_CPU->GetRegister(CPU::Register::R7));
                
                ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##R2Label", "R2: 0x%04X", m_Emulator.m_CPU->GetRegister(CPU::Register::R2));
                ImGui::SameLine(); ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##R5Label", "R5: 0x%04X", m_Emulator.m_CPU->GetRegister(CPU::Register::R5));
                ImGui::SameLine(); ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##RFLabel", "RF: 0x%04X", m_Emulator.m_CPU->GetRegister(CPU::Register::RF));

            }
            ImGui::EndChild();

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
            ImGui::BeginChild(4, { width, width / (16.f/9.f)}, ImGuiChildFlags_Borders);
            m_Screen.Swap(m_Emulator.GetScreenPixel(UI::Screen::Width, UI::Screen::Height));
            m_Emulator.FireVBlankInterrupt();
            ImGui::Image(m_Screen.GetTexture(), { width, width / (16.f / 9.f) }, { width-20, 300-33 });
            ImGui::EndChild();
            ImGui::PopStyleVar();

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
            ImGui::BeginChild(2, { width, -(51)}, ImGuiChildFlags_Borders);
            {
                static std::uint16_t memoryViewStartAddress = 0;
                static std::int32_t memoryViewSearchedAddress = -1;
                constexpr int pageSize = 288;
                
                ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 0, 0 });
                if (ImGui::BeginTable("##Memory View", 17, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchSame))
                {
                    ImGui::TableSetupColumn("Offset");
                    ImGui::TableSetupColumn("00");
                    ImGui::TableSetupColumn("01");
                    ImGui::TableSetupColumn("02");
                    ImGui::TableSetupColumn("03");
                    ImGui::TableSetupColumn("04");
                    ImGui::TableSetupColumn("05");
                    ImGui::TableSetupColumn("06");
                    ImGui::TableSetupColumn("07");
                    ImGui::TableSetupColumn("08");
                    ImGui::TableSetupColumn("09");
                    ImGui::TableSetupColumn("0A");
                    ImGui::TableSetupColumn("0B");
                    ImGui::TableSetupColumn("0C");
                    ImGui::TableSetupColumn("0D");
                    ImGui::TableSetupColumn("0E");
                    ImGui::TableSetupColumn("0F");
                    ImGui::TableHeadersRow();
                
                    for (int row = memoryViewStartAddress; row < (int)m_Emulator.m_RAM.GetSize() && row < memoryViewStartAddress + pageSize*1.4; row += 16)
                    {
                        ImGui::TableNextRow();
                
                        ImGui::TableSetColumnIndex(0);
                        ImGui::TextDisabled("0x%04X", row);
                
                        for (int column = 0; column < 16; column++)
                        {
                            ImGui::TableSetColumnIndex(column + 1);
                            const std::uint16_t addressofCell = static_cast<std::uint16_t>(row + column);
                
                            char buf[5] = { 0 };
                            std::snprintf(buf, 5, "0x%02X", m_Emulator.m_RAM.GetMemory(addressofCell));
                
                            ImGui::PushID(addressofCell);
                
                            int colors = 0;
                            if (addressofCell != memoryViewSearchedAddress)
                            {
                                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0)); // normal
                                ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0, 0, 0, 0)); // hover
                                ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0, 0, 0, 0)); // active (blue)
                                colors = 3;
                            }
                
                            ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
                            if (ImGui::InputText("##MemoryCellText", buf, 5, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue))
                            {
                                int num;
                                const int result = std::sscanf(buf, "%x", &num);
                                if (result != 0 && result != EOF)
                                {
                                    m_Emulator.m_RAM.SetMemory(addressofCell, static_cast<std::uint8_t>(num));
                                }
                            }
                            ImGui::PopStyleColor(colors);
                            ImGui::PopID();
                        }
                    }
                    ImGui::EndTable();
                }
                ImGui::PopStyleVar();
                
                
                static char buf[7] = "Search";
                if (ImGui::InputText("##SearchInput", buf, 7, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    int address;
                    const int result = std::sscanf(buf, "%x", &address);
                    if (result == EOF)
                    {
                        memoryViewSearchedAddress = -1;
                    }
                    else if (result != 0)
                    {
                        memoryViewSearchedAddress = address;
                        memoryViewStartAddress = static_cast<std::uint16_t>(address - (address % 16));
                    }
                }
                
                ImGui::SameLine();
                if (ImGui::Button("Previous Page", { 145, 0 }))
                {
                    memoryViewStartAddress = static_cast<std::uint16_t>(std::max(0, memoryViewStartAddress - pageSize / 2));
                }
                ImGui::SameLine();
                if (ImGui::Button("Next Page", { 145, 0 }))
                {
                    memoryViewStartAddress = static_cast<std::uint16_t>(std::min(65520, memoryViewStartAddress + pageSize / 2));
                }
            }
            ImGui::EndChild();
            ImGui::PopStyleVar();


            //ImGui::SetNextWindowSize({ width, 100 });
            //ImGui::SetNextWindowPos({ 20, 560 });
            ImGui::BeginChild("ButtonWindow", { width, 47 }, ImGuiChildFlags_Borders);
            {
                ImGui::SameLine();
                if (ImGui::Button(!m_Emulator.m_IsExecuting ? "Execute" : "Pause", { 100, 30 }))
                {
                    if (m_Emulator.m_IsExecuting)
                        m_Emulator.StopExecution();
                    else
                        m_Emulator.StartExecution();
                }

                ImGui::SameLine();
                if (ImGui::Button("Step", { 100, 30 }))
                {
                    m_Emulator.Step();
                }
                ImGui::SameLine(); if (ImGui::Button("Reload", { 100, 30 })) m_ShouldReset = true;
            }
            ImGui::EndChild();


            ImGui::SetCursorPos({ ImGui::GetWindowWidth() - instrWidth, cursorY});
            ImGui::BeginChild("Instructions", { 0, 0 }, ImGuiChildFlags_Borders);
            {
                if (!m_Disassembler.GetErrorMsg().empty())
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
                    ImGui::TextWrapped("%s", m_Disassembler.GetErrorMsg().c_str());
                    ImGui::PopStyleColor();
                }

                if (!m_Emulator.m_CPU->GetErrorMsg().empty())
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
                    ImGui::TextWrapped("%s", m_Emulator.m_CPU->GetErrorMsg().c_str());
                    ImGui::PopStyleColor();
                }

                const auto& sourceInstructions = m_Disassembler.GetSourceInstructions();
            
                for (auto& instr : sourceInstructions)
                {
                    if (instr.first == m_Emulator.m_CPU->GetProgramCounter())
                    {
                        const ImU32 color = m_Emulator.m_CPU->GetErrorMsg().empty() ? IM_COL32(0, 255, 0, 255) : IM_COL32(255, 0, 0, 255);
                        ImGui::PushStyleColor(ImGuiCol_Text, color);
                        ImGui::LabelText("##InstructionLabel", "-> %s", instr.second.c_str());
                        ImGui::PopStyleColor();
                    }
                    else
                    {
                        ImGui::LabelText("##InstructionLabel", "%s", instr.second.c_str());
                    }
                }
            }
            ImGui::EndChild();
        }
    };
}

#endif // EMULATOR_HPP