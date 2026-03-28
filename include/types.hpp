#pragma once
#include <algorithm>
#include <array>
#include <cstdint>

struct CPUState
{
    std::array<uint32_t, 32> gpr;
    uint32_t pc;
    bool halt;

    CPUState() : pc(0), halt(false)
    {
        std::fill(gpr.begin(), gpr.end(), 0);
    }
};

enum Opcode : uint8_t
{
    OPCODE_LOAD = 0x03,
    OPCODE_OP_IMM = 0x13,
    OPCODE_AUIPC = 0x17,
    OPCODE_STORE = 0x23,
    OPCODE_OP = 0x33,
    OPCODE_LUI = 0x37,
    OPCODE_BRANCH = 0x63,
    OPCODE_JALR = 0x67,
    OPCODE_JAL = 0x6F,
    OPCODE_SYSTEM = 0x73,
    OPCODE_MISC_MEM = 0x0F
};

enum Funct3 : uint8_t
{

    FUNCT3_ADD_SUB = 0x0,
    FUNCT3_SLL = 0x1,
    FUNCT3_SLT = 0x2,
    FUNCT3_SLTU = 0x3,
    FUNCT3_XOR = 0x4,
    FUNCT3_SRL_SRA = 0x5,
    FUNCT3_OR = 0x6,
    FUNCT3_AND = 0x7,

    FUNCT3_BEQ = 0x0,
    FUNCT3_BNE = 0x1,
    FUNCT3_BLT = 0x4,
    FUNCT3_BGE = 0x5,
    FUNCT3_BLTU = 0x6,
    FUNCT3_BGEU = 0x7,

    FUNCT3_LB = 0x0,
    FUNCT3_LH = 0x1,
    FUNCT3_LW = 0x2,
    FUNCT3_LBU = 0x4,
    FUNCT3_LHU = 0x5,

    FUNCT3_SB = 0x0,
    FUNCT3_SH = 0x1,
    FUNCT3_SW = 0x2
};

enum Funct7 : uint8_t
{
    FUNCT7_STANDARD = 0x00,
    FUNCT7_SPECIAL = 0x20
};

enum class InstrType
{
    R,
    I,
    S,
    B,
    U,
    J,
    UNKNOWN
};

struct DecodedInst
{
    uint32_t raw;

    Opcode opcode;
    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2;
    Funct3 funct3;
    Funct7 funct7;
    int32_t imm;
    InstrType type;

    DecodedInst()
        : raw(0), opcode(OPCODE_SYSTEM), rd(0), rs1(0), rs2(0),
          funct3(FUNCT3_ADD_SUB), funct7(FUNCT7_STANDARD), imm(0),
          type(InstrType::UNKNOWN)
    {
    }
};
