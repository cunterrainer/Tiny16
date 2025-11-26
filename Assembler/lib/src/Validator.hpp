#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>
#include <cstdint>
#include <optional>
#include <string_view>
#include <unordered_map>

#include "Parser.hpp"
#include "Instruction.hpp"

static const std::unordered_map<std::string_view, std::uint8_t> m_RegisterMap = {
    { "R0", 0 },
    { "R1", 1 },
    { "R2", 2 },
    { "R3", 3 },
    { "R4", 4 },
    { "R5", 5 },
    { "R6", 6 },
    { "R7", 7 },
    { "R8", 8 },
    { "R9", 9 },
    { "RA", 10 },
    { "RB", 11 },
    { "RC", 12 },
    { "RD", 13 },
    { "RE", 14 },
    { "RSP", 15 },
    { "RBP", 16 }
};


bool IsValidRegister(std::string s);
std::optional<Instruction> LookupOpcode(std::string opcode);
Result<void, ASMError> ValidateInstruction(const ParsedInstruction& instr, const std::unordered_set<std::string>& labels);
Result<void, ASMError> ValidateAllInstructions(const std::pair<std::vector<ParsedInstruction>, std::unordered_set<std::string>>& parseResult);

#endif // VALIDATOR_H