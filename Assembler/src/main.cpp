#include <vector>
#include <format>
#include <string>
#include <iostream>
#include <string_view>

#include "Parser.hpp"
#include "Utility.hpp"
#include "Validator.hpp"

#include "Utility/Result.hpp"

int main()
{
    try
    {
        const std::string file = "examples/example1.s";
        const std::vector<std::string> sourceLines = ReadFile(file).Unwrap();

        const std::vector<ParsedInstruction> instructions = ParseSourceCode(sourceLines).Unwrap();

        for (const auto& instr : instructions)
        {
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