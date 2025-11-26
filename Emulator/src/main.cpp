#if defined(__EMSCRIPTEN__)
    #include <emscripten/emscripten.h>
#endif

#include "raylib.h"
#include "rlImGui.h"

#include "UI/Application.hpp"

int main()
{
    InitWindow(1280, 720, "Tiny16-Emulator");
    SetExitKey(KEY_NULL);
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    rlImGuiSetup(true);
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;

    UI::Application app;

    #if defined(__EMSCRIPTEN__)
        emscripten_set_main_loop_arg(UI::Application::EmscriptenLoopCallback, &app, 0, 1);
    #else
        while (!WindowShouldClose())
        {
            app.Run();
        }
    #endif

    rlImGuiShutdown();
    TerminateWindow();
    return 0;
}