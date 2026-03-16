// source/main.cpp
#include <iostream>
#include "machine.hpp"
#include "elf_loader.hpp"

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <elf_file>" << std::endl;
        return 1;
    }

    Machine machine(64 * 1024);

    uint32_t entry_point = 0;
    if (!ElfLoader::load(argv[1], machine, entry_point)) {
        return 1;
    }

    machine.set_start_address(entry_point);

    std::cout << "\nStarting execution..." << std::endl;
    machine.run();

    machine.dump_registers(std::cout);

    return 0;
}
