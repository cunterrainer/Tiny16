#include "doctest/doctest.h"

#include "Parser.hpp"

TEST_CASE("Testing Tokenize()")
{
    auto tokens = Tokenize("MOV $1, R0");
    CHECK(tokens[0] == "MOV");
    CHECK(tokens[1] == "$1");
    CHECK(tokens[2] == "R0");

    tokens = Tokenize("MOV $1,R0");
    CHECK(tokens[0] == "MOV");
    CHECK(tokens[1] == "$1");
    CHECK(tokens[2] == "R0");
    
    tokens = Tokenize("MOV $123456789, R6");
    CHECK(tokens[0] == "MOV");
    CHECK(tokens[1] == "$123456789");
    CHECK(tokens[2] == "R6");
    
    tokens = Tokenize("MOV $123456789,R6");
    CHECK(tokens[0] == "MOV");
    CHECK(tokens[1] == "$123456789");
    CHECK(tokens[2] == "R6");

    tokens = Tokenize("MOV R1,R6");
    CHECK(tokens[0] == "MOV");
    CHECK(tokens[1] == "R1");
    CHECK(tokens[2] == "R6");

    tokens = Tokenize("JMP R0");
    CHECK(tokens[0] == "JMP");
    CHECK(tokens[1] == "R0");

    tokens = Tokenize("HLT");
    CHECK(tokens[0] == "HLT");

    tokens = Tokenize("HLT");
    CHECK(tokens[0] == "HLT");
}