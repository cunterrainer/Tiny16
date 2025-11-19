#include <cstdio>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include <utility>
#include <optional>
#include <algorithm>
#include <iostream>

#include "imgui.h"
#include "raygui.h"
#include "raylib.h"
#include "rlImGui.h"

#include "Core/CPU.hpp"
#include "Core/RAM.hpp"
#include "File.hpp"
#include "Core/PROM.hpp"
#include "UI/Emulator.hpp"
#include "UI/ScriptIDE.hpp"
#include "Disassembler/Disassembler.hpp"

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
    SetExitKey(KEY_NULL);
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    rlImGuiSetup(true);

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;

    UI::ScriptIDE f;
    UI::Emulator emu(cpu, ram, dism);
    while (!WindowShouldClose())
    {
        if (cpu.IsExecuting() && (emu.ExecuteClicked() || emu.StepClicked()))
        {
           cpu.Clock();
           emu.SetStep(false);
        }

        BeginDrawing();
        rlImGuiBegin();

        ImGui::SetNextWindowPos({ 0, 0 });
        ImGui::SetNextWindowSize({ (float)GetScreenWidth(), (float)GetScreenHeight()});
        ImGui::Begin("##MainWindow", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar);
        {
            ImGui::BeginTabBar("EmulatorTabs");
            {
                if (ImGui::BeginTabItem("Emulator"))
                {
                    emu.Show();
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Editor"))
                {
                    f.Show();
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }
        ImGui::End();

        rlImGuiEnd();
        EndDrawing();
    }

    rlImGuiShutdown();		// cleans up ImGui
    TerminateWindow();
    return 0;
}