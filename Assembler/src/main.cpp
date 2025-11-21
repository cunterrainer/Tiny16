// TODO: Write Assembler tests

#include <vector>
#include <format>
#include <string>
#include <fstream>
#include <iostream>
#include <string_view>

#include "Parser.hpp"
#include "Utility.hpp"
#include "Assembler.hpp"
#include "Validator.hpp"
#include "Intermediate.hpp"

#include "Utility/Result.hpp"

void WriteBinaryFile(const std::vector<InstructionMC>& instructions)
{
    std::ofstream file("a.tiny16", std::ofstream::binary);
    
    for (const auto& instr : instructions)
    {
        // TODO: Write endianess properly
        file << instr.opcode;
        if (instr.op1.type == OperandTypeMC::Register)
        {
            file.put(std::get<std::uint8_t>(instr.op1.value));
        }
        else if (instr.op1.type == OperandTypeMC::Intermediate)
        {
            // writes it in little endian order
            const std::uint16_t v = std::get<std::uint16_t>(instr.op1.value);
            file.put(static_cast<std::uint8_t>(v & 0xFF));
            file.put(static_cast<std::uint8_t>((v >> 8) & 0xFF));
        }

        if (instr.op2.type == OperandTypeMC::Register)
        {
            file.put(std::get<std::uint8_t>(instr.op2.value));
        }
        else if (instr.op2.type == OperandTypeMC::Intermediate)
        {
            // writes it in little endian order
            const std::uint16_t v = std::get<std::uint16_t>(instr.op2.value);
            file.put(static_cast<std::uint8_t>(v & 0xFF));
            file.put(static_cast<std::uint8_t>((v >> 8) & 0xFF));
        }
    }
}

int main()
{
    try
    {
        const std::string file = "examples/example3.s";
        const std::vector<std::string> sourceLines = ReadFile(file).Unwrap();

        const auto parseResult = ParseSourceCode(sourceLines).Unwrap();
        const std::vector<ParsedInstruction> parsedInstructions = parseResult.first;
        const std::unordered_set<std::string> labels = parseResult.second;

        std::vector<InstructionIR> intermediateInstructions;
        intermediateInstructions.reserve(parsedInstructions.size());

        for (const auto& instr : parsedInstructions)
        {
            ValidateInstruction(instr, labels).Unwrap();
            intermediateInstructions.push_back(LowerInstruction(instr));
            //const ValidationResult result = ValidateInstruction(instr);
            //if (!result.valid)
            //{
            //    std::cerr << result.errorMsg << std::format("\nLine: {}, File: {}", instr.lineNumber, file) << std::endl;
            //    return -1;
            //}
        }

        const std::vector<InstructionMC> assembledInstructions = AssembleInstructions(intermediateInstructions).Unwrap();
        WriteBinaryFile(assembledInstructions);
    }
    catch (const ASMError& e)
    {
        std::cerr << "Error in line: " << e.LineNumber() << ' ' << e.What() << std::endl;
    }
    catch (const Err& e)
    {
        std::cerr << e.What() << std::endl;
        return 1;
    }
    catch (const std::logic_error& e) // Happens for bugs in debug mode mainly in Intermedite.cpp
    {
        std::cerr << "Logic error occured: " << e.what() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Unhandled exception occured: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception occured" << std::endl;
        return 1;
    }
    return 0;
}