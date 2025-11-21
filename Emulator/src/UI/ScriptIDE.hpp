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
    private:
        TextEditor::LanguageDefinition GetLangugeDefiniton() const
        {
            TextEditor::LanguageDefinition langDef;

            constexpr const char* const keywords[] = {
                "mov", "add", "sub", "jmp", "je", "cmp", "hlt", "load", "store",
                "MOV", "ADD", "SUB", "JMP", "JE", "CMP", "HLT", "LOAD", "STORE"
            };
            for (const char* k : keywords)
                langDef.mKeywords.emplace(k);

            langDef.mTokenRegexStrings.emplace_back("[+-]?0[xX][0-9a-fA-F]+", TextEditor::PaletteIndex::Number);
            langDef.mTokenRegexStrings.emplace_back("0[bB][01]+", TextEditor::PaletteIndex::Number);
            langDef.mTokenRegexStrings.emplace_back("[+-]?[0-9]+", TextEditor::PaletteIndex::Number);
            langDef.mTokenRegexStrings.emplace_back("[rR][0-7]", TextEditor::PaletteIndex::KnownIdentifier);
            langDef.mTokenRegexStrings.emplace_back("[a-zA-Z_][a-zA-Z0-9_:]*", TextEditor::PaletteIndex::Identifier);
            langDef.mTokenRegexStrings.emplace_back("[\\$,]", TextEditor::PaletteIndex::Punctuation);

            langDef.mCommentStart = "/*";
            langDef.mCommentEnd = "*/";
            langDef.mSingleLineComment = "#";
            langDef.mCaseSensitive = true;
            langDef.mAutoIndentation = true;

            langDef.mName = "TASM";
            return langDef;
        }
    public:
        ScriptIDE()
        {
            m_TextEditor.SetLanguageDefinition(GetLangugeDefiniton());
            m_TextEditor.SetShowWhitespaces(false);
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