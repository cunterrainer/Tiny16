#include <vector>
#include <cstdint>
#include <optional>

#include "raylib.h"

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

    Rectangle button = { 10, 150, 100, 50 };
    bool execute = false;
    std::string ins;

    while (!WindowShouldClose())
    {
        if (ins != "HLT" && execute)
        {
           ins = cpu.Clock();
           //execute = false;
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawText(TextFormat("R0: 0x%04X", cpu.GetRegister(CPU::Register::R0)),  10, 10, 20, WHITE);
        DrawText(TextFormat("R1:  0x%04X", cpu.GetRegister(CPU::Register::R1)), 10, 30, 20, WHITE);
        DrawText(TextFormat("R2: 0x%04X", cpu.GetRegister(CPU::Register::R2)),  10, 50, 20, WHITE);
        DrawText(TextFormat("R3: 0x%04X", cpu.GetRegister(CPU::Register::R3)),  10, 70, 20, WHITE);
        DrawText(TextFormat("R4: 0x%04X", cpu.GetRegister(CPU::Register::R4)),  10, 90, 20, WHITE);

        DrawText(TextFormat("R5: 0x%04X", cpu.GetRegister(CPU::Register::R5)),  130, 10, 20, WHITE);
        DrawText(TextFormat("R6: 0x%04X", cpu.GetRegister(CPU::Register::R6)),  130, 30, 20, WHITE);
        DrawText(TextFormat("R7: 0x%04X", cpu.GetRegister(CPU::Register::R7)),  130, 50, 20, WHITE);
        DrawText(TextFormat("R8: 0x%04X", cpu.GetRegister(CPU::Register::R8)),  130, 70, 20, WHITE);
        DrawText(TextFormat("R9: 0x%04X", cpu.GetRegister(CPU::Register::R9)),  130, 90, 20, WHITE);

        DrawText(TextFormat("RF: 0x%04X", cpu.GetRegister(CPU::Register::RF)), 250, 10, 20, WHITE);
        DrawText(TextFormat("PC: %d", cpu.GetProgramCounter()), 250, 30, 20, WHITE);

        DrawText(ins.c_str(), 10, 120, 20, WHITE);
        DrawRectangleRec(button, WHITE);
        if (CheckCollisionPointRec(GetMousePosition(), button) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            execute = true;
        EndDrawing();
    }


    TerminateWindow();
    return 0;
}