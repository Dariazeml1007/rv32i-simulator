#include <iostream>
#include "machine.hpp"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <binary_file>" << std::endl;
        return 1;
    }


    Machine machine(64 * 1024);


    if (!machine.load_binary(argv[1], 0)) {
        return 1;
    }


    std::cout << "\nFirst 16 bytes of memory:" << std::endl;
    machine.dump_memory(std::cout, 0, 16);


    machine.set_start_address(0);


    std::cout << "\nStarting execution" << std::endl;
    machine.run();


    machine.dump_registers(std::cout);

    return 0;
}
