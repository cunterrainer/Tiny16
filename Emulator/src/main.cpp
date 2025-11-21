#include "imgui.h"
#include "raygui.h"
#include "raylib.h"
#include "rlImGui.h"

#include "UI/Emulator.hpp"
#include "UI/ScriptIDE.hpp"

int main()
{
    InitWindow(1280, 720, "Tiny16-Emulator");
    SetExitKey(KEY_NULL);
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    rlImGuiSetup(true);

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;

    UI::ScriptIDE f;
    UI::Emulator emu;
    while (!WindowShouldClose())
    {
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
                    emu.Render();
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

    rlImGuiShutdown();
    TerminateWindow();
    return 0;
}