#include <vector>
#include <cstdint>
#include <cstdlib>
#include <utility>
#include <optional>

#include "imgui.h"
#include "raygui.h"
#include "raylib.h"
#include "rlImGui.h"

#include "CPU.hpp"
#include "RAM.hpp"
#include "File.hpp"
#include "PROM.hpp"
#include "Disassembler.hpp"

int main()
{
    std::optional<std::vector<std::uint8_t>> e = LoadFile("examples/a.tiny16"); // TODO add error message in release
    if (!e.has_value())
        return EXIT_FAILURE;

    const Disassembler dism(e.value());
    const PROM prom(e.value());
    RAM ram;
    CPU cpu(prom, ram);

    InitWindow(1280, 720, "Tiny16-Emulator");
    rlImGuiSetup(true);

    bool execute = false;
    bool step = false;
    std::string ins;

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;

    while (!WindowShouldClose())
    {
        if (cpu.IsExecuting() && (execute || step))
        {
           cpu.Clock();
           step = false;
        }

        BeginDrawing();
        ClearBackground({ 14, 14, 14, 255 });

        rlImGuiBegin();
        
        float width = 900;
        ImGui::SetNextWindowSize({ width, 100 });
        ImGui::SetNextWindowPos({ 20, 20 });
        ImGui::Begin("Register", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        {
            float item_width = 100;

            ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##R0Label", "R0: 0x%04X", cpu.GetRegister(CPU::Register::R0));
            ImGui::SameLine(); ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##R3Label", "R3: 0x%04X", cpu.GetRegister(CPU::Register::R3));
            ImGui::SameLine(); ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##R6Label", "R6: 0x%04X", cpu.GetRegister(CPU::Register::R6));
            ImGui::SameLine(); ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##PCLabel", "PC: 0x%04X", cpu.GetProgramCounter());

            ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##R1Label", "R1: 0x%04X", cpu.GetRegister(CPU::Register::R1));
            ImGui::SameLine(); ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##R4Label", "R4: 0x%04X", cpu.GetRegister(CPU::Register::R4));
            ImGui::SameLine(); ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##R7Label", "R7: 0x%04X", cpu.GetRegister(CPU::Register::R7));

            ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##R2Label", "R2: 0x%04X", cpu.GetRegister(CPU::Register::R2));
            ImGui::SameLine(); ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##R5Label", "R5: 0x%04X", cpu.GetRegister(CPU::Register::R5));
            ImGui::SameLine(); ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##RFLabel", "RF: 0x%04X", cpu.GetRegister(CPU::Register::RF));

        }
        ImGui::End();

        ImGui::SetNextWindowSize({ width, 400 });
        ImGui::SetNextWindowPos({ 20, 140 });
        ImGui::Begin("Memory", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        {
            if (true && ImGui::BeginTable("##Memory View", 17, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchSame))
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

                for (int row = 0; row < ram.GetSize() && row < 512; row += 16)
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextDisabled("0x%04X", row);

                    for (int column = 0; column < 16; column++)
                    {
                        ImGui::TableSetColumnIndex(column + 1);
                        ImGui::Text("0x%02X", ram.GetMemory(row + column));
                    }
                }
                ImGui::EndTable();
            }
        }
        ImGui::End();

        ImGui::SetNextWindowSize({ width, 100 });
        ImGui::SetNextWindowPos({ 20, 560 });
        ImGui::Begin("ButtonWindow", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration);
        {
            ImGui::SameLine(); if (ImGui::Button("Execute", { 100, 30 })) execute = !execute;
            ImGui::SameLine(); if (ImGui::Button("Pause", { 100, 30 })) execute = false;
            ImGui::SameLine(); ImGui::Button("Stop", { 100, 30 });
            ImGui::SameLine(); if (ImGui::Button("Step", { 100, 30 })) step = true;
        }
        ImGui::End();

        ImGui::SetNextWindowSize({ 300, 640 });
        ImGui::SetNextWindowPos({ width + 40, 20 });
        ImGui::Begin("Instructions", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        {
            const auto& sourceInstructions = dism.GetSourceInstructions();

            for (auto& instr : sourceInstructions)
            {
                if (instr.first == cpu.GetProgramCounter())
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                    ImGui::LabelText("##InstructionLabel", "-> %s", instr.second.c_str());
                    ImGui::PopStyleColor();
                }
                else
                {
                    ImGui::LabelText("##InstructionLabel", "%s", instr.second.c_str());
                }
            }

        }
        ImGui::End();

        rlImGuiEnd();
        EndDrawing();
    }

    rlImGuiShutdown();		// cleans up ImGui
    TerminateWindow();
    return 0;
}