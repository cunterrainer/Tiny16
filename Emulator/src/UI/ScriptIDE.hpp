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
        ScriptIDE()
        {
            TextEditor::LanguageDefinition langDef = m_TextEditor.GetLanguageDefinition();
            const char* const cppKeywords[] = {
                "mov", "add", "sub", "jmp", "je", "cmp", "hlt", "load", "store", "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
                "MOV", "ADD", "SUB", "JMP", "JE", "CMP", "HLT", "LOAD", "STORE", "R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7"
            };
            for (auto& k : cppKeywords)
                langDef.mKeywords.insert(k);

            langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>("\\$[+-]?[0-9]+", TextEditor::PaletteIndex::Number));
            langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>("\\$0[bB][01]+", TextEditor::PaletteIndex::Number));
            langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>("\\$[+-]?0[xX][0-9a-fA-F]+", TextEditor::PaletteIndex::Number));
            langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*:", TextEditor::PaletteIndex::Identifier));

            langDef.mSingleLineComment = "#";
            langDef.mCaseSensitive = true;
            langDef.mAutoIndentation = true;

            langDef.mName = "TASM";

            m_TextEditor.SetLanguageDefinition(langDef);
        }

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