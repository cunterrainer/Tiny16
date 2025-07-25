#include "doctest/doctest.h"

#include "Intermediate.hpp"

OpcodeIR ToOpcode(const std::string& str);
TEST_CASE("Testing ToOpcode")
{
    SUBCASE("Valid opcodes")
    {
        CHECK(ToOpcode("MOV")   == OpcodeIR::MOV);
        CHECK(ToOpcode("mov")   == OpcodeIR::MOV);
        CHECK(ToOpcode("MoV")   == OpcodeIR::MOV);

        CHECK(ToOpcode("ADD")   == OpcodeIR::ADD);
        CHECK(ToOpcode("add")   == OpcodeIR::ADD);
        CHECK(ToOpcode("AdD")   == OpcodeIR::ADD);

        CHECK(ToOpcode("SUB")   == OpcodeIR::SUB);
        CHECK(ToOpcode("sub")   == OpcodeIR::SUB);

        CHECK(ToOpcode("CMP")   == OpcodeIR::CMP);
        CHECK(ToOpcode("cmp")   == OpcodeIR::CMP);

        CHECK(ToOpcode("JMP")   == OpcodeIR::JMP);
        CHECK(ToOpcode("jmp")   == OpcodeIR::JMP);

        CHECK(ToOpcode("JE")    == OpcodeIR::JE);
        CHECK(ToOpcode("je")    == OpcodeIR::JE);

        CHECK(ToOpcode("HLT")   == OpcodeIR::HLT);
        CHECK(ToOpcode("hlt")   == OpcodeIR::HLT);

        CHECK(ToOpcode("LOAD")  == OpcodeIR::LOAD);
        CHECK(ToOpcode("load")  == OpcodeIR::LOAD);

        CHECK(ToOpcode("STORE") == OpcodeIR::STORE);
        CHECK(ToOpcode("store") == OpcodeIR::STORE);
    }
    
    
    SUBCASE("Invalid opcode")
    {
        CHECK_THROWS_AS(ToOpcode("foo"), std::logic_error);
        CHECK_THROWS_AS(ToOpcode(""), std::logic_error);
        CHECK_THROWS_AS(ToOpcode("123"), std::logic_error);
    }
}


std::uint16_t ParseIntermediate(const std::string& operand);
TEST_CASE("Testing ParseIntermediate()")
{
    SUBCASE("Valid decimal inputs")
    {
        CHECK(ParseIntermediate("$123") == 123);
        CHECK(ParseIntermediate("$+123") == 123);
        CHECK(ParseIntermediate("$-123") == static_cast<uint16_t>(-123));
    }

    SUBCASE("Valid hexadecimal inputs")
    {
        CHECK(ParseIntermediate("$0x1A") == 0x1A);
        CHECK(ParseIntermediate("$+0x1A") == 0x1A);
        CHECK(ParseIntermediate("$-0x1A") == static_cast<uint16_t>(-0x1A));
        CHECK(ParseIntermediate("$0XFF") == 0xFF);
    }

    SUBCASE("Valid binary inputs")
    {
        CHECK(ParseIntermediate("$0b1010") == 0b1010);
        CHECK(ParseIntermediate("$+0b1010") == 0b1010);
        CHECK(ParseIntermediate("$0B11111111") == 255);
    }

    SUBCASE("Edge cases with clamping")
    {
        CHECK(ParseIntermediate("$32767") == 32767);
        CHECK(ParseIntermediate("$-32768") == static_cast<uint16_t>(-32768));
        CHECK(ParseIntermediate("$65535") == 65535); // Clamped
        CHECK(ParseIntermediate("$99999") == 65535); // Too large, clamped
        CHECK(ParseIntermediate("$-99999") == static_cast<uint16_t>(-32768)); // Too small, clamped
    }

    SUBCASE("Invalid inputs")
    {
        CHECK_THROWS_AS(ParseIntermediate("123"), std::logic_error);     // No $
        CHECK_THROWS_AS(ParseIntermediate(""), std::logic_error);        // Empty
        CHECK_THROWS_AS(ParseIntermediate("$"), std::logic_error);       // Missing value
        CHECK_THROWS_AS(ParseIntermediate("$0xGHI"), std::logic_error);  // Invalid hex
        CHECK_THROWS_AS(ParseIntermediate("$0b2101"), std::logic_error); // Invalid binary
        CHECK_THROWS_AS(ParseIntermediate("$-"), std::logic_error);      // Just minus
        CHECK_THROWS_AS(ParseIntermediate("$-0b1010"), std::logic_error);
    }
}


namespace {
    bool operator==(const Operand& lhs, const Operand& rhs) {
        return lhs.type == rhs.type && lhs.value == rhs.value;
    }
}

std::optional<Operand> ParseOperand(const std::string& str);
TEST_CASE("Testing ParseOperand()")
{
    SUBCASE("Valid registers")
    {
        auto a = ParseOperand("R0").value();
        CHECK(a == Operand{ OperandType::Register, 0 });

        a = ParseOperand("r1").value();
        CHECK(a == Operand{ OperandType::Register, 1 });

        a = ParseOperand("R7").value();
        CHECK(a == Operand{ OperandType::Register, 7 });
    }

    SUBCASE("Valid immediates")
    {
        auto a = ParseOperand("$0").value();
        CHECK(a == Operand{ OperandType::Immediate, 0 });

        a = ParseOperand("$42").value();
        CHECK(a == Operand{ OperandType::Immediate, 42 });

        a = ParseOperand("$-1").value();
        CHECK(a == Operand{ OperandType::Immediate, static_cast<uint16_t>(-1) });

        a = ParseOperand("$+99").value();
        CHECK(a == Operand{ OperandType::Immediate, 99 });

        a = ParseOperand("$0x10").value();
        CHECK(a == Operand{ OperandType::Immediate, 0x10 });

        a = ParseOperand("$0XFF").value();
        CHECK(a == Operand{ OperandType::Immediate, 0xFF });

        a = ParseOperand("$0b1010").value();
        CHECK(a == Operand{ OperandType::Immediate, 0b1010 });

        a = ParseOperand("$0B1111").value();
        CHECK(a == Operand{ OperandType::Immediate, 0b1111 });
    }

    SUBCASE("Invalid inputs")
    {
        CHECK_FALSE(ParseOperand("").has_value());
        CHECK_FALSE(ParseOperand("X1").has_value());
        CHECK_FALSE(ParseOperand("12").has_value());
        CHECK_THROWS_AS(ParseOperand("$"), std::logic_error);
        CHECK_THROWS_AS(ParseOperand("Rx"), std::logic_error);;
        CHECK_THROWS_AS(ParseOperand("$0xGG"), std::logic_error);
        CHECK_THROWS_AS(ParseOperand("$0b212"), std::logic_error);
        CHECK_THROWS_AS(ParseOperand("$-"), std::logic_error);
        CHECK_THROWS_AS(ParseOperand("$0x"), std::logic_error);
        CHECK_THROWS_AS(ParseOperand("$0b"), std::logic_error);
    }
}


TEST_CASE("Testing LowerInstruction()")
{
    SUBCASE("Valid instruction")
    {
        ParsedInstruction pi {
            .label = "start",
            .opcode = "MOV",
            .lhs = "R1",
            .rhs = "$42"
        };

        auto inst = LowerInstruction(pi);

        CHECK(inst.label == "start");
        CHECK(inst.opcode == OpcodeIR::MOV);
        
        auto op1 = inst.op1;
        CHECK(op1.type == OperandType::Register);
        CHECK(op1.value == 1);

        auto op2 = inst.op2;
        CHECK(op2.type == OperandType::Immediate);
        CHECK(op2.value == 42);
    }

    SUBCASE("Invalid lhs operand throws")
    {
        ParsedInstruction pi {
            .label = "label",
            .opcode = "ADD",
            .lhs = "InvalidOp",
            .rhs = "R2"
        };

        CHECK_THROWS_AS(LowerInstruction(pi), std::logic_error);
    }

    SUBCASE("Invalid rhs operand throws")
    {
        ParsedInstruction pi {
            .label = "label",
            .opcode = "SUB",
            .lhs = "R1",
            .rhs = "???"
        };

        CHECK_THROWS_AS(LowerInstruction(pi), std::logic_error);
    }

    SUBCASE("Invalid opcode throws from ToOpcode")
    {
        ParsedInstruction pi {
            .label = "lbl",
            .opcode = "NOTEXIST",
            .lhs = "R1",
            .rhs = "R2"
        };

        CHECK_THROWS_AS(LowerInstruction(pi), std::logic_error);
    }
}