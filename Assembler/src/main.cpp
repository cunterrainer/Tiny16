#include <vector>
#include <string>
#include <optional>
#include <string_view>

#include "Parser.hpp"
#include "Utility.hpp"

int main()
{
    std::optional<std::vector<std::string>> sourceLines = ReadFile("examples/example2.s");
    if (!sourceLines)
        return 1;

    std::vector<Instruction> instructions = ParseSourceCode(sourceLines.value());
    return 0;
}