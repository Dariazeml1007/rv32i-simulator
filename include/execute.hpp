#pragma once

#include "memory.hpp"
#include "types.hpp"
#include <array>
#include <cstdint>

class Executor
{
public:
    static void execute(const DecodedInst& instr, CPUState& state,
                        Memory& memory);
    static void handle_ecall(CPUState& state, Memory& memory);

    static void execute_r_type(const DecodedInst& instr, CPUState& state);
    static void execute_i_type(const DecodedInst& instr, CPUState& state,
                               Memory& memory);
    static void execute_s_type(const DecodedInst& instr, CPUState& state,
                               Memory& memory);
    static void execute_b_type(const DecodedInst& instr, CPUState& state);
    static void execute_u_type(const DecodedInst& instr, CPUState& state);
    static void execute_j_type(const DecodedInst& instr, CPUState& state);

    static void set_register(std::array<uint32_t, 32>& gpr, uint8_t index,
                             uint32_t value);
    static int32_t sign_extend(int32_t val, int bits);
};

