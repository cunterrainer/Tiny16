#include <vector>
#include <cstdint>
#include <optional>

#include "imgui.h"
#include "raygui.h"
#include "raylib.h"
#include "rlImGui.h"

#include "CPU.hpp"
#include "File.hpp"
#include "PROM.hpp"

int main()
{
    std::optional<std::vector<std::uint8_t>> e = LoadFile("examples/example2.ty");
    if (!e.has_value())
        return EXIT_FAILURE;

    PROM prom(e.value());
    CPU cpu(prom);

    InitWindow(1280, 720, "Tiny16-Emulator");
    rlImGuiSetup(true);

    Rectangle button = { 10, 150, 100, 50 };
    bool execute = false;
    bool step = false;
    std::string ins;

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;

    while (!WindowShouldClose())
    {
        if (ins != "HLT" && (execute || step))
        {
           ins = cpu.Clock();
           step = false;
        }

        BeginDrawing();
        ClearBackground({ 14, 14, 14, 255 });

        rlImGuiBegin();
        
        float width = 900;
        ImGui::SetNextWindowSize({ width, 100 });
        ImGui::SetNextWindowPos({ 20, 20 });
        ImGui::Begin("Register", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        
        float item_width = 100;
                           ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##R0Label", "R0: 0x%04X", cpu.GetRegister(CPU::Register::R0));
        ImGui::SameLine(); ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##R1Label", "R1: 0x%04X", cpu.GetRegister(CPU::Register::R1));
        ImGui::SameLine(); ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##R2Label", "R2: 0x%04X", cpu.GetRegister(CPU::Register::R2));
        ImGui::SameLine(); ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##R3Label", "R3: 0x%04X", cpu.GetRegister(CPU::Register::R3));
        
                           ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##R4Label", "R4: 0x%04X", cpu.GetRegister(CPU::Register::R4));
        ImGui::SameLine(); ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##R5Label", "R5: 0x%04X", cpu.GetRegister(CPU::Register::R5));
        ImGui::SameLine(); ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##R6Label", "R6: 0x%04X", cpu.GetRegister(CPU::Register::R6));
        ImGui::SameLine(); ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##R7Label", "R7: 0x%04X", cpu.GetRegister(CPU::Register::R7));
        
                           ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##R8Label", "R8: 0x%04X", cpu.GetRegister(CPU::Register::R8));
        ImGui::SameLine(); ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##R9Label", "R9: 0x%04X", cpu.GetRegister(CPU::Register::R9));
        ImGui::SameLine(); ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##RFLabel", "RF: 0x%04X", cpu.GetRegister(CPU::Register::RF));
        ImGui::SameLine(); ImGui::SetNextItemWidth(item_width); ImGui::LabelText("##PCLabel", "PC: 0x%04X", cpu.GetProgramCounter());
        ImGui::End();

        ImGui::SetNextWindowSize({ width, 400 });
        ImGui::SetNextWindowPos({ 20, 140 });
        ImGui::Begin("Memory", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        ImGui::End();

        ImGui::SetNextWindowSize({ width, 100 });
        ImGui::SetNextWindowPos({ 20, 560 });
        ImGui::Begin("ButtonWindow", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration);
        ImGui::SameLine(); if (ImGui::Button("Execute", { 100, 30 })) execute = !execute;
        ImGui::SameLine(); if (ImGui::Button("Pause", { 100, 30 })) execute = false;
        ImGui::SameLine(); ImGui::Button("Stop", { 100, 30 });
        ImGui::SameLine(); if (ImGui::Button("Step", { 100, 30 })) step = true;
        ImGui::End();

        ImGui::SetNextWindowSize({ 300, 640 });
        ImGui::SetNextWindowPos({ width + 40, 20 });
        ImGui::Begin("Instructions", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        ImGui::LabelText("##CurrentInstructionLabel", "%s", ins.c_str());
        ImGui::End();

        rlImGuiEnd();

        //DrawText(TextFormat("R0: 0x%04X", cpu.GetRegister(CPU::Register::R0)),  10, 10, 20, WHITE);
        //DrawText(TextFormat("R1:  0x%04X", cpu.GetRegister(CPU::Register::R1)), 10, 30, 20, WHITE);
        //DrawText(TextFormat("R2: 0x%04X", cpu.GetRegister(CPU::Register::R2)),  10, 50, 20, WHITE);
        //DrawText(TextFormat("R3: 0x%04X", cpu.GetRegister(CPU::Register::R3)),  10, 70, 20, WHITE);
        //DrawText(TextFormat("R4: 0x%04X", cpu.GetRegister(CPU::Register::R4)),  10, 90, 20, WHITE);
        //
        //DrawText(TextFormat("R5: 0x%04X", cpu.GetRegister(CPU::Register::R5)),  130, 10, 20, WHITE);
        //DrawText(TextFormat("R6: 0x%04X", cpu.GetRegister(CPU::Register::R6)),  130, 30, 20, WHITE);
        //DrawText(TextFormat("R7: 0x%04X", cpu.GetRegister(CPU::Register::R7)),  130, 50, 20, WHITE);
        //DrawText(TextFormat("R8: 0x%04X", cpu.GetRegister(CPU::Register::R8)),  130, 70, 20, WHITE);
        //DrawText(TextFormat("R9: 0x%04X", cpu.GetRegister(CPU::Register::R9)),  130, 90, 20, WHITE);
        //
        //DrawText(TextFormat("RF: 0x%04X", cpu.GetRegister(CPU::Register::RF)), 250, 10, 20, WHITE);
        //DrawText(TextFormat("PC: %d", cpu.GetProgramCounter()), 250, 30, 20, WHITE);
        //
        //DrawText(ins.c_str(), 10, 120, 20, WHITE);
        //DrawRectangleRec(button, WHITE);
        //if (CheckCollisionPointRec(GetMousePosition(), button) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        //    execute = true;
        EndDrawing();
    }

    rlImGuiShutdown();		// cleans up ImGui
    TerminateWindow();
    return 0;
}