#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"

#include "UI/Emulator.hpp"
#include "UI/ScriptIDE.hpp"

namespace UI
{
    class Application
    {
    private:
        UI::ScriptIDE m_IDE;
        UI::Emulator m_Emulator;
    public:
        static void EmscriptenLoopCallback(void* arg)
        {
            // Cast the generic pointer back to our specific Application class
            Application* app = static_cast<Application*>(arg);

            // Call the member function
            app->Run();
        }

        void Run()
        {
            BeginDrawing();
            rlImGuiBegin();

            ImGui::SetNextWindowPos({ 0, 0 });
            ImGui::SetNextWindowSize({ (float)GetScreenWidth(), (float)GetScreenHeight() });
            ImGui::Begin("##MainWindow", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar);
            {
                if (ImGui::BeginMenuBar())
                {
                    if (ImGui::BeginMenu("Menu"))
                    {
                        if (ImGui::BeginMenu("Examples"))
                        {
                            if (ImGui::MenuItem("Color pattern"))
                            {
                                m_IDE.LoadExample("Color pattern");
                            }

                            if (ImGui::MenuItem("Scrolling interface"))
                            {
                                m_IDE.LoadExample("Scrolling interface");
                            }

                            if (ImGui::MenuItem("Scrolling digital wave"))
                            {
                                m_IDE.LoadExample("Scrolling digital wave");
                            }

                            if (ImGui::MenuItem("Hypnotic Ripples"))
                            {
                                m_IDE.LoadExample("Hypnotic Ripples");
                            }

                            if (ImGui::MenuItem("Diamond tunnel"))
                            {
                                m_IDE.LoadExample("Diamond tunnel");
                            }

                            if (ImGui::MenuItem("Fractal Scroll"))
                            {
                                m_IDE.LoadExample("Fractal Scroll");
                            }

                            if (ImGui::MenuItem("Manhatten tunnel"))
                            {
                                m_IDE.LoadExample("Manhatten tunnel");
                            }

                            if (ImGui::MenuItem("Hyperbolic plaid"))
                            {
                                m_IDE.LoadExample("Hyperbolic plaid");
                            }

                            if (ImGui::MenuItem("Kaleidoscope"))
                            {
                                m_IDE.LoadExample("Kaleidoscope");
                            }

                            if (ImGui::MenuItem("Perspective grid"))
                            {
                                m_IDE.LoadExample("Perspective grid");
                            }
                            ImGui::EndMenu();
                        }
                        ImGui::EndMenu();
                    }

                    ImGui::EndMenuBar();
                }

                ImGui::BeginTabBar("EmulatorTabs");
                {
                    if (ImGui::BeginTabItem("Emulator"))
                    {
                        m_Emulator.Render();
                        ImGui::EndTabItem();

                        if (m_Emulator.ShouldReset())
                        {
                            m_Emulator.LoadProgram(m_IDE.GetMachineCode()); // TODO add other option if loaded in by external file, but works for now
                            m_Emulator.SetShouldReset(false);
                        }
                    }

                    if (ImGui::BeginTabItem("Editor"))
                    {
                        m_IDE.Show();
                        ImGui::EndTabItem();

                        switch (m_IDE.GetState())
                        {
                        case UI::ScriptIDE::State::CompiledAndRun:
                        {
                            ImGui::SetTabItemClosed("Editor");
                            m_Emulator.LoadProgram(m_IDE.GetMachineCode());
                            m_IDE.ResetState();
                            m_Emulator.StartExecution();
                            break;
                        }
                        case UI::ScriptIDE::State::CompiledAndDebug:
                        {
                            ImGui::SetTabItemClosed("Editor");
                            m_Emulator.LoadProgram(m_IDE.GetMachineCode());
                            m_IDE.ResetState();
                            break;
                        }
                        case UI::ScriptIDE::State::Compiled:
                        {
                            m_Emulator.LoadProgram(m_IDE.GetMachineCode());
                            m_IDE.ResetState();
                            break;
                        }
                        case UI::ScriptIDE::State::None:
                            break;
                        }
                    }
                }
                ImGui::EndTabBar();
            }
            ImGui::End();
            rlImGuiEnd();
            EndDrawing();
        }
    };
}

#endif // APPLICATION_HPP