#include <vector>
#include <format>
#include <string>
#include <optional>
#include <iostream>
#include <string_view>

#include "Parser.hpp"
#include "Utility.hpp"
#include "Validator.hpp"

int main()
{
    const std::string file = "Assembler/examples/example2.s";
    std::optional<std::vector<std::string>> sourceLines = ReadFile(file);
    if (!sourceLines)
        return 1;

    std::vector<Instruction> instructions = ParseSourceCode(sourceLines.value());
    for (const auto& instr : instructions)
    {
        const ValidationResult result = ValidateInstruction(instr);
        if (!result.valid)
        {
            std::cerr << result.errorMsg << std::format("\nLine: {}, File: {}", instr.lineNumber, file) << std::endl;
            return -1;
        }
    }
    return 0;
}