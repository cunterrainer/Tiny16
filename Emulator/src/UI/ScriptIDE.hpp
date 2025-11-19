#ifndef SCRIPT_IDE_HPP
#define SCRIPT_IDE_HPP

#include <string>

#include "imgui.h"
#include "imgui_stdlib.h"

namespace UI
{
    class ScriptIDE
    {
    private:
        std::string m_Content;
    public:
        void Show()
        {
            ImGui::BeginChild("Editor", { 0, 0 });
            {
                ImGui::Button("Compile"); ImGui::SameLine(); ImGui::Button("Compile and Run"); ImGui::SameLine(); ImGui::Button("Compile and Debug");
                
                const ImVec2 childWindowSize = ImGui::GetWindowSize();
                const float editorHeight = childWindowSize.y - ImGui::GetCursorPosY();
                ImGui::InputTextMultiline("##Editor", &m_Content, { childWindowSize.x, editorHeight });
            }
            ImGui::EndChild();
        }
    };
}

#endif // SCRIPT_IDE_HPP