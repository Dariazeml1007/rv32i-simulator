#pragma once

#include "cpu.hpp"
#include "memory.hpp"
#include <cstdint>
#include <iosfwd>
#include <string>

class Machine
{
private:
    Memory memory;
    CPU cpu;

public:
    Machine(size_t memory_size = 64 * 1024);

    Memory& get_memory()
    {
        return memory;
    }
    CPU& get_cpu()
    {
        return cpu;
    }
    const Memory& get_memory() const
    {
        return memory;
    }
    const CPU& get_cpu() const
    {
        return cpu;
    }

    bool load_binary(const std::string& filename, uint32_t base_addr = 0);

    void reset();
    void step();
    void run();
    void run_with_limit(uint32_t max_steps);

    void set_start_address(uint32_t addr);

    bool is_halted() const;
    uint32_t get_pc() const;

    void dump_memory(std::ostream& os, uint32_t start, uint32_t end) const;

    void dump_registers(std::ostream& os) const;
};

