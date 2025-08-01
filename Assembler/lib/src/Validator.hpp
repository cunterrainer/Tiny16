#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>

#include "Parser.hpp"
#include "Instruction.hpp"

std::optional<Instruction> LookupOpcode(const std::string& opcode);
Result<void> ValidateInstruction(const ParsedInstruction& instr, const std::unordered_set<std::string>& labels);

#endif // VALIDATOR_H