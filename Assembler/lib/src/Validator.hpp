#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>

#include "Parser.hpp"

struct ValidationResult
{
    bool valid;
    std::string errorMsg;
};

ValidationResult ValidateInstruction(const ParsedInstruction& instr);

#endif // VALIDATOR_H