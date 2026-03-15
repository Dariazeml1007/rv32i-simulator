#include "execute.hpp"
#include <iostream>

void Executor::set_register(std::array<uint32_t, 32>& gpr, uint8_t index, uint32_t value) {
    if (index != 0) {
        gpr[index] = value;
    }
}

int32_t Executor::sign_extend(int32_t val, int bits) {
    int32_t mask = 1 << (bits - 1);
    return (val ^ mask) - mask;
}

void Executor::execute(const DecodedInst& instr, CPUState& state, Memory& memory) {
    switch (instr.type) {
        case InstrType::R:
            execute_r_type(instr, state);
            break;
        case InstrType::I:
            execute_i_type(instr, state, memory);
            break;
        case InstrType::S:
            execute_s_type(instr, state, memory);
            break;
        case InstrType::B:
            execute_b_type(instr, state);
            break;
        case InstrType::U:
            execute_u_type(instr, state);
            break;
        case InstrType::J:
            execute_j_type(instr, state);
            break;
        default:
            std::cerr << "Unknown instruction type" << std::endl;
            state.halt = true;
            break;
    }
}

void Executor::execute_r_type(const DecodedInst& instr, CPUState& state) {
    uint32_t a = state.gpr[instr.rs1];
    uint32_t b = state.gpr[instr.rs2];
    uint32_t result = 0;

     std::cout << "R-type: rs1=" << (int)instr.rs1
              << " (" << a << "), rs2=" << (int)instr.rs2
              << " (" << b << "), funct3=" << (int)instr.funct3
              << ", funct7=" << (int)instr.funct7 << std::endl;

    switch (instr.funct3) {
        case FUNCT3_ADD_SUB:
            if (instr.funct7 == FUNCT7_STANDARD) result = a + b;
            else if (instr.funct7 == FUNCT7_SPECIAL) result = a - b;
            break;
        case FUNCT3_SLL:
            result = a << (b & 0x1F);
            break;
        case FUNCT3_SLT:
            result = (static_cast<int32_t>(a) < static_cast<int32_t>(b)) ? 1 : 0;
            break;
        case FUNCT3_SLTU:
            result = (a < b) ? 1 : 0;
            break;
        case FUNCT3_XOR:
            result = a ^ b;
            break;
        case FUNCT3_SRL_SRA:
            if (instr.funct7 == FUNCT7_STANDARD) {
                result = a >> (b & 0x1F);
            } else {
                result = static_cast<int32_t>(a) >> (b & 0x1F);
            }
            break;
        case FUNCT3_OR:
            result = a | b;
            break;
        case FUNCT3_AND:
            result = a & b;
            break;
    }

    set_register(state.gpr, instr.rd, result);
}

void Executor::execute_i_type(const DecodedInst& instr, CPUState& state, Memory& memory) {
    uint32_t a = state.gpr[instr.rs1];
    uint32_t result = 0;

        std::cout << "DEBUG I-type: opcode=0x" << std::hex << instr.opcode
          << ", funct3=0x" << instr.funct3
          << ", rs1=x" << std::dec << (int)instr.rs1
          << "=" << state.gpr[instr.rs1]
          << ", imm=" << instr.imm
          << ", rd=x" << (int)instr.rd << std::endl;

    switch (instr.opcode) {
        case OPCODE_OP_IMM:
            switch (instr.funct3) {
                case FUNCT3_ADD_SUB:
                    result = a + instr.imm;
                    break;
                case FUNCT3_SLL:
                    result = a << (instr.imm & 0x1F);
                    break;
                case FUNCT3_SLT:
                    result = (static_cast<int32_t>(a) < instr.imm) ? 1 : 0;
                    break;
                case FUNCT3_SLTU:
                    result = (a < static_cast<uint32_t>(instr.imm)) ? 1 : 0;
                    break;
                case FUNCT3_XOR:
                    result = a ^ instr.imm;
                    break;
                case FUNCT3_SRL_SRA:
                    if (instr.funct7 == FUNCT7_STANDARD) {
                        result = a >> (instr.imm & 0x1F);
                    } else {
                        result = static_cast<int32_t>(a) >> (instr.imm & 0x1F);
                    }
                    break;
                case FUNCT3_OR:
                    result = a | instr.imm;
                    break;
                case FUNCT3_AND:
                    result = a & instr.imm;
                    break;
            }
            set_register(state.gpr, instr.rd, result);
            break;

        case OPCODE_LOAD: {
            uint32_t addr = a + instr.imm;
            switch (instr.funct3) {
                case FUNCT3_LB:
                    result = sign_extend(memory.read<uint8_t>(addr), 8);
                    break;
                case FUNCT3_LH:
                    result = sign_extend(memory.read<uint16_t>(addr), 16);
                    break;
                case FUNCT3_LW:
                    result = memory.read<uint32_t>(addr);
                    break;
                case FUNCT3_LBU:
                    result = memory.read<uint8_t>(addr);
                    break;
                case FUNCT3_LHU:
                    result = memory.read<uint16_t>(addr);
                    break;
                default:
                    std::cerr << "Unknown load funct3" << std::endl;
                    state.halt = true;
                    return;
            }
            set_register(state.gpr, instr.rd, result);
            break;
        }

        case OPCODE_JALR: {
            uint32_t next_pc = state.pc + 4;
            state.pc = (state.gpr[instr.rs1] + instr.imm) & ~1;
            set_register(state.gpr, instr.rd, next_pc);
            break;  
        }

            case OPCODE_SYSTEM:  

            state.halt = true;
            return;
        default:
            std::cerr << "Unknown I-type opcode" << std::endl;
            state.halt = true;
            break;
    }
}

void Executor::execute_s_type(const DecodedInst& instr, CPUState& state, Memory& memory) {
    uint32_t addr = state.gpr[instr.rs1] + instr.imm;
    uint32_t val = state.gpr[instr.rs2];

    switch (instr.funct3) {
        case FUNCT3_SB:
            memory.write<uint8_t>(addr, val & 0xFF);
            break;
        case FUNCT3_SH:
            memory.write<uint16_t>(addr, val & 0xFFFF);
            break;
        case FUNCT3_SW:
            memory.write<uint32_t>(addr, val);
            break;
        default:
            std::cerr << "Unknown store funct3" << std::endl;
            state.halt = true;
            break;
    }
}

void Executor::execute_b_type(const DecodedInst& instr, CPUState& state) {
    uint32_t a = state.gpr[instr.rs1];
    uint32_t b = state.gpr[instr.rs2];
    bool condition = false;

    switch (instr.funct3) {
        case FUNCT3_BEQ:
            condition = (a == b);
            break;
        case FUNCT3_BNE:
            condition = (a != b);
            break;
        case FUNCT3_BLT:
            condition = (static_cast<int32_t>(a) < static_cast<int32_t>(b));
            break;
        case FUNCT3_BGE:
            condition = (static_cast<int32_t>(a) >= static_cast<int32_t>(b));
            break;
        case FUNCT3_BLTU:
            condition = (a < b);
            break;
        case FUNCT3_BGEU:
            condition = (a >= b);
            break;
        default:
            std::cerr << "Unknown branch funct3" << std::endl;
            state.halt = true;
            return;
    }

    if (condition) {
        state.pc += instr.imm;
        
    } else {
        state.pc += 4;
    }
}

void Executor::execute_u_type(const DecodedInst& instr, CPUState& state) {
    if (instr.opcode == OPCODE_LUI) {
        set_register(state.gpr, instr.rd, instr.imm);
    } else if (instr.opcode == OPCODE_AUIPC) {
        set_register(state.gpr, instr.rd, state.pc + instr.imm);
    }
    state.pc += 4;
}

void Executor::execute_j_type(const DecodedInst& instr, CPUState& state) {
    uint32_t next_pc = state.pc + 4;
    set_register(state.gpr, instr.rd, next_pc);
    state.pc += instr.imm;
    
}
