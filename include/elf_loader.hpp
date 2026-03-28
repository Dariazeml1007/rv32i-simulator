#pragma once
#include <cstdint>
#include <string>

class Machine;

class ElfLoader
{
public:
    static bool load(const std::string& filename, Machine& machine,
                     uint32_t& entry_point);
};
