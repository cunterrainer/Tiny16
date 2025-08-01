#include "doctest/doctest.h"

#include <cstdint>
#include <variant>
#include <stdexcept>

#include "Validator.hpp"
#include "Intermediate.hpp"

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
    bool operator==(const OperandIR& lhs, const OperandIR& rhs) {
        return lhs.type == rhs.type && lhs.value == rhs.value;
    }
}

OperandIR ParseOperand(const std::string& str);
TEST_CASE("Testing ParseOperand()")
{
    SUBCASE("Valid registers")
    {
        auto a = ParseOperand("R0");
        CHECK(a == OperandIR{ OperandTypeIR::Register, (uint8_t)0 });

        a = ParseOperand("r1");
        CHECK(a == OperandIR{ OperandTypeIR::Register, (uint8_t)1 });

        a = ParseOperand("R7");
        CHECK(a == OperandIR{ OperandTypeIR::Register, (uint8_t)7 });
    }

    SUBCASE("Valid immediates")
    {
        auto a = ParseOperand("$0");
        CHECK(a == OperandIR{ OperandTypeIR::Intermediate, (uint16_t)0 });

        a = ParseOperand("$42");
        CHECK(a == OperandIR{ OperandTypeIR::Intermediate, (uint16_t)42 });

        a = ParseOperand("$-1");
        CHECK(a == OperandIR{ OperandTypeIR::Intermediate, static_cast<uint16_t>(-1) });

        a = ParseOperand("$+99");
        CHECK(a == OperandIR{ OperandTypeIR::Intermediate, (uint16_t)99 });

        a = ParseOperand("$0x10");
        CHECK(a == OperandIR{ OperandTypeIR::Intermediate, (uint16_t)0x10 });

        a = ParseOperand("$0XFF");
        CHECK(a == OperandIR{ OperandTypeIR::Intermediate, (uint16_t)0xFF });

        a = ParseOperand("$0b1010");
        CHECK(a == OperandIR{ OperandTypeIR::Intermediate, (uint16_t)0b1010 });

        a = ParseOperand("$0B1111");
        CHECK(a == OperandIR{ OperandTypeIR::Intermediate, (uint16_t)0b1111 });
    }

    SUBCASE("Invalid inputs")
    {
        CHECK_THROWS_AS(ParseOperand(""), std::logic_error);
        CHECK_THROWS_AS(ParseOperand("X1"), std::logic_error);
        CHECK_THROWS_AS(ParseOperand("12"), std::logic_error);
        CHECK_THROWS_AS(ParseOperand("$"), std::logic_error);
        CHECK_THROWS_AS(ParseOperand("$0xGG"), std::logic_error);
        CHECK_THROWS_AS(ParseOperand("$0b212"), std::logic_error);
        CHECK_THROWS_AS(ParseOperand("$-"), std::logic_error);
        CHECK_THROWS_AS(ParseOperand("$0x"), std::logic_error);
        CHECK_THROWS_AS(ParseOperand("$0b"), std::logic_error);
        CHECK_THROWS_AS(ParseOperand("Hello"), std::logic_error);
    }
}


OperandIR ParseOperandJmp(const std::string& str);
TEST_CASE("Testing ParseOperandJmp()")
{
    SUBCASE("Valid registers")
    {
        auto a = ParseOperandJmp("R0");
        CHECK(a == OperandIR{ OperandTypeIR::Register, (uint8_t)0 });

        a = ParseOperandJmp("r1");
        CHECK(a == OperandIR{ OperandTypeIR::Register, (uint8_t)1 });

        a = ParseOperandJmp("R7");
        CHECK(a == OperandIR{ OperandTypeIR::Register, (uint8_t)7 });
    }

    SUBCASE("Valid Label")
    {
        auto a = ParseOperandJmp("IsEqual");
        CHECK(a == OperandIR{ OperandTypeIR::Label, std::string("IsEqual")});

        a = ParseOperandJmp("ToLower");
        CHECK(a == OperandIR{ OperandTypeIR::Label, std::string("ToLower") });
    }
}


OpcodeIR GetOpcodeIR(Opcode opcode, OperandTypeIR op1);
TEST_CASE("Test GetOpcodeIR()")
{
    SUBCASE("MOV")
    {
        CHECK(GetOpcodeIR(Opcode::MOV, OperandTypeIR::Register) == OpcodeIR::MOV_REG_TO_REG);
        CHECK(GetOpcodeIR(Opcode::MOV, OperandTypeIR::Intermediate) == OpcodeIR::MOV_IMM_TO_REG);
    }

    SUBCASE("ADD")
    {
        CHECK(GetOpcodeIR(Opcode::ADD, OperandTypeIR::Register) == OpcodeIR::ADD_REG_TO_REG);
        CHECK(GetOpcodeIR(Opcode::ADD, OperandTypeIR::Intermediate) == OpcodeIR::ADD_IMM_TO_REG);
    }

    SUBCASE("SUB")
    {
        CHECK(GetOpcodeIR(Opcode::SUB, OperandTypeIR::Register) == OpcodeIR::SUB_REG_TO_REG);
        CHECK(GetOpcodeIR(Opcode::SUB, OperandTypeIR::Intermediate) == OpcodeIR::SUB_IMM_TO_REG);
    }

    SUBCASE("CMP")
    {
        CHECK(GetOpcodeIR(Opcode::CMP, OperandTypeIR::Register) == OpcodeIR::CMP_REG_TO_REG);
        CHECK(GetOpcodeIR(Opcode::CMP, OperandTypeIR::Intermediate) == OpcodeIR::CMP_IMM_TO_REG);
    }

    SUBCASE("HLT")
    {
        CHECK(GetOpcodeIR(Opcode::HLT, OperandTypeIR::None) == OpcodeIR::HLT);
    }

    SUBCASE("JMP")
    {
        CHECK(GetOpcodeIR(Opcode::JMP, OperandTypeIR::Register) == OpcodeIR::JMP_REG);
        CHECK(GetOpcodeIR(Opcode::JMP, OperandTypeIR::Label) == OpcodeIR::JMP_LABEL);
    }

    SUBCASE("JE")
    {
        CHECK(GetOpcodeIR(Opcode::JE, OperandTypeIR::Register) == OpcodeIR::JE_REG);
        CHECK(GetOpcodeIR(Opcode::JE, OperandTypeIR::Label) == OpcodeIR::JE_LABEL);
    }

    SUBCASE("LOAD")
    {
        CHECK(GetOpcodeIR(Opcode::LOAD, OperandTypeIR::Register) == OpcodeIR::LOAD);
        CHECK(GetOpcodeIR(Opcode::LOAD, OperandTypeIR::Intermediate) == OpcodeIR::LOAD);
    }

    SUBCASE("STORE")
    {
        CHECK(GetOpcodeIR(Opcode::STORE, OperandTypeIR::Register) == OpcodeIR::STORE);
        CHECK(GetOpcodeIR(Opcode::STORE, OperandTypeIR::Intermediate) == OpcodeIR::STORE);
    }

    CHECK_THROWS_AS(GetOpcodeIR((Opcode)100, (OperandTypeIR)100), std::logic_error);
}


TEST_CASE("Testing LowerInstruction() Valid")
{
    SUBCASE("MOV Intermediate to Register")
    {
        ParsedInstruction pi {
            .label = "start",
            .opcode = "MOV",
            .lhs = "$42",
            .rhs = "R1"
        };

        auto inst = LowerInstruction(pi);

        CHECK(inst.size == 4);
        CHECK(inst.label == "start");
        CHECK(inst.opcode == OpcodeIR::MOV_IMM_TO_REG);
        CHECK(inst.op1.type == OperandTypeIR::Intermediate);
        CHECK(inst.op2.type == OperandTypeIR::Register);
        CHECK(std::get<std::uint16_t>(inst.op1.value) == 42);
        CHECK(std::get<std::uint8_t>(inst.op2.value) == 1);
    }

    SUBCASE("MOV Register to Register")
    {
        ParsedInstruction pi {
            .label = "",
            .opcode = "MOV",
            .lhs = "R2",
            .rhs = "R0"
        };

        auto inst = LowerInstruction(pi);

        CHECK(inst.size == 3);
        CHECK(inst.label.empty());
        CHECK(inst.opcode == OpcodeIR::MOV_REG_TO_REG);
        CHECK(inst.op1.type == OperandTypeIR::Register);
        CHECK(inst.op2.type == OperandTypeIR::Register);
        CHECK(std::get<std::uint8_t>(inst.op1.value) == 2);
        CHECK(std::get<std::uint8_t>(inst.op2.value) == 0);
    }

    SUBCASE("ADD Immediate to Register")
    {
        ParsedInstruction pi {
            .label = "",
            .opcode = "ADD",
            .lhs = "$10",
            .rhs = "R3"
        };

        auto inst = LowerInstruction(pi);

        CHECK(inst.size == 4);
        CHECK(inst.opcode == OpcodeIR::ADD_IMM_TO_REG);
        CHECK(inst.op1.type == OperandTypeIR::Intermediate);
        CHECK(inst.op2.type == OperandTypeIR::Register);
        CHECK(std::get<std::uint16_t>(inst.op1.value) == 10);
        CHECK(std::get<std::uint8_t>(inst.op2.value) == 3);
    }

    SUBCASE("ADD Register to Register")
    {
        ParsedInstruction pi{
            .label = "hello",
            .opcode = "ADD",
            .lhs = "R0",
            .rhs = "R3"
        };

        auto inst = LowerInstruction(pi);

        CHECK(inst.size == 3);
        CHECK(inst.label == "hello");
        CHECK(inst.opcode == OpcodeIR::ADD_REG_TO_REG);
        CHECK(inst.op1.type == OperandTypeIR::Register);
        CHECK(inst.op2.type == OperandTypeIR::Register);
        CHECK(std::get<std::uint8_t>(inst.op1.value) == 0);
        CHECK(std::get<std::uint8_t>(inst.op2.value) == 3);
    }

    SUBCASE("SUB Register to Register")
    {
        ParsedInstruction pi {
            .label = "loop",
            .opcode = "SUB",
            .lhs = "R5",
            .rhs = "R6"
        };

        auto inst = LowerInstruction(pi);

        CHECK(inst.size == 3);
        CHECK(inst.label == "loop");
        CHECK(inst.opcode == OpcodeIR::SUB_REG_TO_REG);
        CHECK(inst.op1.type == OperandTypeIR::Register);
        CHECK(inst.op2.type == OperandTypeIR::Register);
        CHECK(std::get<std::uint8_t>(inst.op1.value) == 5);
        CHECK(std::get<std::uint8_t>(inst.op2.value) == 6);
    }

    SUBCASE("SUB Intermediate to Register")
    {
        ParsedInstruction pi{
            .label = "loop",
            .opcode = "SUB",
            .lhs = "$0b0101",
            .rhs = "R6"
        };

        auto inst = LowerInstruction(pi);

        CHECK(inst.size == 4);
        CHECK(inst.label == "loop");
        CHECK(inst.opcode == OpcodeIR::SUB_IMM_TO_REG);
        CHECK(inst.op1.type == OperandTypeIR::Intermediate);
        CHECK(inst.op2.type == OperandTypeIR::Register);
        CHECK(std::get<std::uint16_t>(inst.op1.value) == 0b0101);
        CHECK(std::get<std::uint8_t>(inst.op2.value) == 6);
    }

    SUBCASE("CMP Immediate to Register")
    {
        ParsedInstruction pi {
            .label = "",
            .opcode = "CMP",
            .lhs = "$0xFF",
            .rhs = "R4"
        };

        auto inst = LowerInstruction(pi);

        CHECK(inst.size == 4);
        CHECK(inst.opcode == OpcodeIR::CMP_IMM_TO_REG);
        CHECK(inst.op1.type == OperandTypeIR::Intermediate);
        CHECK(inst.op2.type == OperandTypeIR::Register);
        CHECK(std::get<std::uint16_t>(inst.op1.value) == 0xFF);
        CHECK(std::get<std::uint8_t>(inst.op2.value) == 4);
    }

    SUBCASE("CMP Register to Register")
    {
        ParsedInstruction pi{
            .label = "",
            .opcode = "CMP",
            .lhs = "R0",
            .rhs = "R4"
        };

        auto inst = LowerInstruction(pi);

        CHECK(inst.size == 3);
        CHECK(inst.opcode == OpcodeIR::CMP_REG_TO_REG);
        CHECK(inst.op1.type == OperandTypeIR::Register);
        CHECK(inst.op2.type == OperandTypeIR::Register);
        CHECK(std::get<std::uint8_t>(inst.op1.value) == 0);
        CHECK(std::get<std::uint8_t>(inst.op2.value) == 4);
    }

    SUBCASE("HLT instruction")
    {
        ParsedInstruction pi {
            .label = "end",
            .opcode = "HLT",
            .lhs = "",
            .rhs = ""
        };

        auto inst = LowerInstruction(pi);

        CHECK(inst.size == 1);
        CHECK(inst.label == "end");
        CHECK(inst.opcode == OpcodeIR::HLT);
        CHECK(inst.op1.type == OperandTypeIR::None);
        CHECK(inst.op2.type == OperandTypeIR::None);
    }

    SUBCASE("JMP to Label")
    {
        ParsedInstruction pi {
            .label = "",
            .opcode = "JMP",
            .lhs = "target",
            .rhs = ""
        };

        auto inst = LowerInstruction(pi);

        CHECK(inst.size == 3);
        CHECK(inst.opcode == OpcodeIR::JMP_LABEL);
        CHECK(inst.op1.type == OperandTypeIR::Label);
        CHECK(inst.op2.type == OperandTypeIR::None);
        CHECK(std::get<std::string>(inst.op1.value) == "target");
    }

    SUBCASE("JE to Register")
    {
        ParsedInstruction pi {
            .label = "",
            .opcode = "JE",
            .lhs = "R7",
            .rhs = ""
        };

        auto inst = LowerInstruction(pi);

        CHECK(inst.size == 2);
        CHECK(inst.opcode == OpcodeIR::JE_REG);
        CHECK(inst.op1.type == OperandTypeIR::Register);
        CHECK(inst.op2.type == OperandTypeIR::None);
        CHECK(std::get<std::uint8_t>(inst.op1.value) == 7);
    }

    SUBCASE("LOAD from address to register")
    {
        ParsedInstruction pi{
            .label = "",
            .opcode = "LOAD",
            .lhs = "$0xFF",
            .rhs = "R0"
        };

        auto inst = LowerInstruction(pi);

        CHECK(inst.size == 4);
        CHECK(inst.opcode == OpcodeIR::LOAD);
        CHECK(inst.op1.type == OperandTypeIR::Intermediate);
        CHECK(inst.op2.type == OperandTypeIR::Register);
        CHECK(std::get<std::uint16_t>(inst.op1.value) == 0xFF);
        CHECK(std::get<std::uint8_t>(inst.op2.value) == 0);
    }

    SUBCASE("STORE from register to address")
    {
        ParsedInstruction pi{
            .label = "",
            .opcode = "STORE",
            .lhs = "R1",
            .rhs = "$0XFF"
        };

        auto inst = LowerInstruction(pi);

        CHECK(inst.size == 4);
        CHECK(inst.opcode == OpcodeIR::STORE);
        CHECK(inst.op1.type == OperandTypeIR::Register);
        CHECK(inst.op2.type == OperandTypeIR::Intermediate);
        CHECK(std::get<std::uint8_t>(inst.op1.value) == 1);
        CHECK(std::get<std::uint16_t>(inst.op2.value) == 0xFF);
    }
}


TEST_CASE("Testing LowerInstruction() Invalid")
{
    SUBCASE("Invalid lhs operand throws")
    {
        ParsedInstruction pi{
            .label = "label",
            .opcode = "ADD",
            .lhs = "InvalidOp",
            .rhs = "R2"
        };

        CHECK_THROWS_AS(LowerInstruction(pi), std::logic_error);
    }

    SUBCASE("Invalid rhs operand throws")
    {
        ParsedInstruction pi{
            .label = "label",
            .opcode = "SUB",
            .lhs = "R1",
            .rhs = "???"
        };

        CHECK_THROWS_AS(LowerInstruction(pi), std::logic_error);
    }

    SUBCASE("Invalid opcode throws from ToOpcode")
    {
        ParsedInstruction pi{
            .label = "lbl",
            .opcode = "NOTEXIST",
            .lhs = "R1",
            .rhs = "R2"
        };

        CHECK_THROWS_AS(LowerInstruction(pi), std::logic_error);
    }
}