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
            }
            catch (const Err& e)
            {
                std::cerr << e.What() << std::endl;
            }
            catch (const std::logic_error& e)
            {
                std::cerr << "Logic error occured: " << e.what() << std::endl;
            }
            catch (const std::exception& e)
            {
                std::cerr << "Unhandled exception occured: " << e.what() << std::endl;
            }
            catch (...)
            {
                std::cerr << "Unknown exception occured" << std::endl;
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
                if (ImGui::Button("Compile"))
                {
                    CompileProgram(State::Compiled);
                }

                ImGui::SameLine();
                if (ImGui::Button("Compile and Run"))
                {
                    CompileProgram(State::CompiledAndRun);
                }

                ImGui::SameLine();
                if (ImGui::Button("Compile and Debug"))
                {
                    CompileProgram(State::CompiledAndDebug);
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