#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>

#include "Parser.hpp"
#include "Instruction.hpp"

std::optional<Instruction> LookupOpcode(const std::string& opcode);
Result<void, ASMError> ValidateInstruction(const ParsedInstruction& instr, const std::unordered_set<std::string>& labels);
Result<void, ASMError> ValidateAllInstructions(const std::pair<std::vector<ParsedInstruction>, std::unordered_set<std::string>>& parseResult);

#endif // VALIDATOR_H