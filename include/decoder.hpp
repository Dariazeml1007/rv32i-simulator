#pragma once

#include "types.hpp"
#include <cstdint>
#include <optional>

class Decoder
{
private:
    static int32_t sign_extend(int32_t val, int bits);

public:
    static std::optional<DecodedInst> decode(uint32_t raw_instr);

    static const char* opcode_to_string(Opcode op);
    static const char* funct3_to_string(Funct3 f3);
    static const char* branch_funct3_to_string(Funct3 f3);
    static const char* instr_type_to_string(InstrType type);
};

