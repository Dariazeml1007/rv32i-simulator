#include <iostream>

#include "decoder.hpp"

int32_t Decoder::sign_extend(int32_t val, int bits)
{
    int32_t mask = 1 << (bits - 1);
    return (val ^ mask) - mask;
}

std::optional<DecodedInst> Decoder::decode(uint32_t raw_instr)
{
    DecodedInst instr;
    instr.raw = raw_instr;

    instr.opcode = static_cast<Opcode>(raw_instr & 0x7F);
    instr.rd = (raw_instr >> 7) & 0x1F;
    instr.rs1 = (raw_instr >> 15) & 0x1F;
    instr.rs2 = (raw_instr >> 20) & 0x1F;
    instr.funct3 = static_cast<Funct3>((raw_instr >> 12) & 0x7);
    instr.funct7 = static_cast<Funct7>((raw_instr >> 25) & 0x7F);

    std::cout << "DECODE: raw=0x" << std::hex << raw_instr << " opcode=0x"
              << (int)instr.opcode << " rd=x" << std::dec << (int)instr.rd
              << " rs1=x" << (int)instr.rs1 << " imm=" << std::hex << "?"
              << std::endl;

    switch (instr.opcode)
    {
    case OPCODE_OP:
        instr.type = InstrType::R;
        instr.imm = 0;
        break;

    case OPCODE_OP_IMM:
    case OPCODE_LOAD:
    case OPCODE_JALR:
        instr.type = InstrType::I;
        instr.imm = sign_extend(static_cast<int32_t>(raw_instr) >> 20, 12);
        break;

    case OPCODE_STORE:
        instr.type = InstrType::S;
        instr.imm = sign_extend(
            ((raw_instr >> 25) << 5) | ((raw_instr >> 7) & 0x1F), 12);
        break;

    case OPCODE_BRANCH:
        instr.type = InstrType::B;
        instr.imm = sign_extend((((raw_instr >> 31) & 0x01) << 12) |
                                    (((raw_instr >> 25) & 0x3F) << 5) |
                                    (((raw_instr >> 8) & 0x0F) << 1) |
                                    (((raw_instr >> 7) & 0x01) << 11),
                                13);
        break;

    case OPCODE_LUI:
    case OPCODE_AUIPC:
        instr.type = InstrType::U;
        instr.imm = raw_instr & 0xFFFFF000;
        break;

    case OPCODE_JAL:
        instr.type = InstrType::J;
        instr.imm = sign_extend(((raw_instr >> 21) & 0x3FF) << 1 |
                                    ((raw_instr >> 20) & 1) << 11 |
                                    ((raw_instr >> 12) & 0xFF) << 12 |
                                    ((raw_instr >> 31) & 1) << 20,
                                21);
        break;
    case OPCODE_SYSTEM:
        instr.type = InstrType::I;
        std::cout << "Decode system\n";
        instr.imm = (raw_instr >> 20) & 0xFFF;

        break;

    case OPCODE_MISC_MEM:
        instr.type = InstrType::I;
        instr.imm = 0;
        break;
    default:

        return std::nullopt;
    }

    return instr;
}

const char* Decoder::opcode_to_string(Opcode op)
{
    switch (op)
    {
    case OPCODE_LOAD:
        return "LOAD";
    case OPCODE_OP_IMM:
        return "OP_IMM";
    case OPCODE_AUIPC:
        return "AUIPC";
    case OPCODE_STORE:
        return "STORE";
    case OPCODE_OP:
        return "OP";
    case OPCODE_LUI:
        return "LUI";
    case OPCODE_BRANCH:
        return "BRANCH";
    case OPCODE_JALR:
        return "JALR";
    case OPCODE_JAL:
        return "JAL";
    default:
        return "UNKNOWN";
    }
}
const char* Decoder::funct3_to_string(Funct3 f3)
{
    switch (f3)
    {

    case FUNCT3_ADD_SUB:
        return "ADD/SUB/ADDI";
    case FUNCT3_SLL:
        return "SLL/SLLI";
    case FUNCT3_SLT:
        return "SLT/SLTI";
    case FUNCT3_SLTU:
        return "SLTU/SLTIU";
    case FUNCT3_XOR:
        return "XOR/XORI";
    case FUNCT3_SRL_SRA:
        return "SRL/SRA/SRLI/SRAI";
    case FUNCT3_OR:
        return "OR/ORI";
    case FUNCT3_AND:
        return "AND/ANDI";
    default:
        return "UNKNOWN";
    }
}

const char* Decoder::branch_funct3_to_string(Funct3 f3)
{
    switch (f3)
    {
    case FUNCT3_BEQ:
        return "BEQ";
    case FUNCT3_BNE:
        return "BNE";
    case FUNCT3_BLT:
        return "BLT";
    case FUNCT3_BGE:
        return "BGE";
    case FUNCT3_BLTU:
        return "BLTU";
    case FUNCT3_BGEU:
        return "BGEU";
    default:
        return "UNKNOWN";
    }
}

const char* Decoder::instr_type_to_string(InstrType type)
{
    switch (type)
    {
    case InstrType::R:
        return "R-type";
    case InstrType::I:
        return "I-type";
    case InstrType::S:
        return "S-type";
    case InstrType::B:
        return "B-type";
    case InstrType::U:
        return "U-type";
    case InstrType::J:
        return "J-type";
    default:
        return "UNKNOWN";
    }
}
