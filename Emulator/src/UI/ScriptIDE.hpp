#ifndef SCRIPT_IDE_HPP
#define SCRIPT_IDE_HPP

#include <vector>
#include <string>
#include <cstdint>
#include <iostream>
#include <exception>
#include <stdexcept>

#include "imgui.h"
#include "imgui_stdlib.h"
#include "TextEditor.h"

#include "Assembler/lib/src/Error.hpp"
#include "Assembler/lib/src/Parser.hpp"
#include "Assembler/lib/src/Validator.hpp"
#include "Assembler/lib/src/Assembler.hpp"
#include "Assembler/lib/src/Intermediate.hpp"
#include "Assembler/lib/src/MachineCodeGenerator.hpp"

#include "UI/Emulator.hpp"

#include "Utility/Result.hpp"

namespace UI
{
    class ScriptIDE
    {
    public:
        enum class State
        {
            None,
                Compiled,
                CompiledAndRun,
                CompiledAndDebug
        };
    private:
        std::string m_Content;
        TextEditor m_TextEditor;

        State m_State;
        std::string m_ErrorMsg;
        std::int64_t m_ErrorLine = -1;
        std::vector<std::uint8_t> m_MachineCode;
    private:
        TextEditor::LanguageDefinition GetLanguageDefinition() const
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


        void CompileProgram(State newState)
        {
            try
            {
                const auto parseResult = ParseSourceCode(m_TextEditor.GetTextLines()).Unwrap();
                ValidateAllInstructions(parseResult).Unwrap();
                const std::vector<InstructionIR> intermediateInstructions = LowerAllInstructions(parseResult.first);
                const std::vector<InstructionMC> assembledInstructions = AssembleInstructions(intermediateInstructions).Unwrap();
                m_MachineCode = GenerateMachineCode(assembledInstructions);
                m_State = newState;
                m_ErrorMsg.clear();
            }
            catch (const ASMError& e)
            {
                m_ErrorLine = (std::int64_t)e.LineNumber();
                m_ErrorMsg = "Error in line: " + std::to_string(m_ErrorLine) + ", " + e.What();
            }
            catch (const Err& e)
            {
                m_ErrorMsg = e.What();
            }
            catch (const std::logic_error& e)
            {
                m_ErrorMsg = "Internal assembler error occured: " + std::string(e.what());
            }
            catch (const std::exception& e)
            {
                m_ErrorMsg = "Unhandled error occured: " + std::string(e.what());
            }
            catch (...)
            {
                m_ErrorMsg = "Unknown error occured:";
            }
        }
    public:
        explicit ScriptIDE()
        {
            m_TextEditor.SetLanguageDefinition(GetLanguageDefinition());
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
                if (ImGui::Button("Compile") || ImGui::Shortcut(ImGuiKey_F5 | ImGuiMod_Ctrl | ImGuiMod_Shift))
                {
                    CompileProgram(State::Compiled);
                }

                ImGui::SameLine();
                if (ImGui::Button("Compile and Run") || ImGui::Shortcut(ImGuiKey_F5 | ImGuiMod_Ctrl))
                {
                    CompileProgram(State::CompiledAndRun);
                }

                ImGui::SameLine();
                if (ImGui::Button("Compile and Debug") || ImGui::Shortcut(ImGuiKey_F5))
                {
                    CompileProgram(State::CompiledAndDebug);
                }

                if (!m_ErrorMsg.empty())
                {
                    ImGui::TextColored({ 255, 0, 0, 255 }, "%s", m_ErrorMsg.c_str());
                }

                if (m_ErrorLine != -1)
                {
                    m_TextEditor.SetCursorPosition({ (int)m_ErrorLine, 0 });

                    const size_t firstCharacter = m_TextEditor.GetCurrentLineText().find_first_not_of(" \t");
                    if (firstCharacter != std::string::npos)
                    {
                        m_TextEditor.SetCursorPosition({ (int)m_ErrorLine, (int)firstCharacter });
                    }
                    m_ErrorLine = -1;
                }
                m_TextEditor.Render("TextEditor");
            }
            ImGui::EndChild();
        }


        inline State GetState() const noexcept
        {
            return m_State;
        }


        inline void ResetState() noexcept
        {
            m_State = State::None;
        }


        inline std::vector<std::uint8_t> GetMachineCode() const noexcept
        {
            return m_MachineCode;
        }
    };
}

#endif // SCRIPT_IDE_HPP