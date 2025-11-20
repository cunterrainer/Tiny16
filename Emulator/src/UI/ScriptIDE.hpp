#ifndef SCRIPT_IDE_HPP
#define SCRIPT_IDE_HPP

#include <string>

#include "imgui.h"
#include "imgui_stdlib.h"
#include "TextEditor.h"

namespace UI
{
    class ScriptIDE
    {
    private:
        std::string m_Content;
        TextEditor m_TextEditor;
    public:
        void Show()
        {
            ImGui::BeginChild("Editor", { 0, 0 });
            {
                if (ImGui::Button("Undo"))
                {
                    m_TextEditor.Undo();
                }

                ImGui::SameLine();
                if (ImGui::Button("Redo"))
                {
                    m_TextEditor.Redo();
                }

                ImGui::SameLine();
                ImGui::Button("Compile"); ImGui::SameLine(); ImGui::Button("Compile and Run"); ImGui::SameLine(); ImGui::Button("Compile and Debug");
                m_TextEditor.Render("Title");
            }
            ImGui::EndChild();
        }
    };
}

#endif // SCRIPT_IDE_HPP