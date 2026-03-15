#include "machine.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>

Machine::Machine(size_t memory_size) : memory(memory_size), cpu() {}

void Machine::reset() {
    cpu.reset();
    
}

void Machine::step() {
    cpu.step(memory);
}

void Machine::run() {
    cpu.run(memory);
}

void Machine::run_with_limit(uint32_t max_steps) {
    cpu.run_with_limit(memory, max_steps);
}

void Machine::set_start_address(uint32_t addr) {
    cpu.set_pc(addr);
}

bool Machine::is_halted() const {
    return cpu.is_halted();
}

uint32_t Machine::get_pc() const {
    return cpu.get_pc();
}

bool Machine::load_binary(const std::string& filename, uint32_t base_addr) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        std::cerr << "Failed to read file: " << filename << std::endl;
        return false;
    }

    
    if (static_cast<size_t>(base_addr) + static_cast<size_t>(size) > memory.size()) {
        std::cerr << "Binary too large for memory. Size: " << size
                  << ", Base: 0x" << std::hex << base_addr
                  << ", Memory size: 0x" << memory.size() << std::endl;
        return false;
    }

    
    for (size_t i = 0; i < static_cast<size_t>(size); ++i) {
        memory.write<uint8_t>(base_addr + static_cast<uint32_t>(i), buffer[i]);
    }

    std::cout << "Loaded " << size << " bytes at address 0x"
              << std::hex << base_addr << std::dec << std::endl;
    return true;
}

void Machine::dump_memory(std::ostream& os, uint32_t start, uint32_t end) const {
    const uint32_t bytes_per_line = 16;

    for (uint32_t addr = start; addr < end; addr += bytes_per_line) {
        
        os << std::hex << std::setw(8) << std::setfill('0') << addr << ": ";

        
        for (uint32_t offset = 0; offset < bytes_per_line; ++offset) {
            if (addr + offset < end && addr + offset < memory.size()) {
                uint8_t byte = memory.read<uint8_t>(addr + offset);
                os << std::hex << std::setw(2) << std::setfill('0')
                   << static_cast<int>(byte) << " ";
            } else {
                os << "   ";
            }

            if (offset == 7) os << " ";
        }

        os << " |";

        
        for (uint32_t offset = 0; offset < bytes_per_line; ++offset) {
            if (addr + offset < end && addr + offset < memory.size()) {
                uint8_t byte = memory.read<uint8_t>(addr + offset);
                if (byte >= 32 && byte <= 126) {
                    os << static_cast<char>(byte);
                } else {
                    os << '.';
                }
            }
        }

        os << "|" << std::endl;
    }
}

void Machine::dump_registers(std::ostream& os) const {
    os << "\n=== Register Dump ===" << std::endl;
    os << "PC: 0x" << std::hex << cpu.get_pc() << std::endl;

    for (int i = 0; i < 32; i += 4) {
        os << "x" << std::dec << std::setw(2) << std::setfill(' ') << i << ": 0x"
           << std::hex << std::setw(8) << std::setfill('0') << cpu.get_register(i) << "  "
           << "x" << (i+1) << ": 0x" << std::setw(8) << std::setfill('0') << cpu.get_register(i+1) << "  "
           << "x" << (i+2) << ": 0x" << std::setw(8) << std::setfill('0') << cpu.get_register(i+2) << "  "
           << "x" << (i+3) << ": 0x" << std::setw(8) << std::setfill('0') << cpu.get_register(i+3)
           << std::endl;
    }
}
