#include <vector>
#include <format>
#include <string>
#include <iostream>
#include <string_view>

#include "Parser.hpp"
#include "Utility.hpp"
#include "Validator.hpp"
#include "Intermediate.hpp"

#include "Utility/Result.hpp"

int main()
{
    try
    {
        const std::string file = "examples/example2.s";
        const std::vector<std::string> sourceLines = ReadFile(file).Unwrap();

        const auto parseResult = ParseSourceCode(sourceLines).Unwrap();
        const std::vector<ParsedInstruction> parsedInstructions = parseResult.first;
        const std::unordered_set<std::string> labels = parseResult.second;

        for (const auto& instr : parsedInstructions)
        {
            ValidateInstruction(instr, labels).Unwrap();
            LowerInstruction(instr);
            //const ValidationResult result = ValidateInstruction(instr);
            //if (!result.valid)
            //{
            //    std::cerr << result.errorMsg << std::format("\nLine: {}, File: {}", instr.lineNumber, file) << std::endl;
            //    return -1;
            //}
        }
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