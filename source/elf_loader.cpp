#include <elfio/elfio.hpp>
#include <iostream>

#include "elf_loader.hpp"
#include "machine.hpp"

bool ElfLoader::load(const std::string& filename, Machine& machine,
                     uint32_t& entry_point)
{
    ELFIO::elfio reader;

    if (!reader.load(filename))
    {
        std::cerr << " Failed to load ELF: " << filename << std::endl;
        return false;
    }

    if (reader.get_machine() != ELFIO::EM_RISCV)
    {
        std::cerr << " Not a RISC-V ELF" << std::endl;
        return false;
    }

    if (reader.get_class() != ELFIO::ELFCLASS32)
    {
        std::cerr << " Not a 32-bit ELF" << std::endl;
        return false;
    }

    entry_point = reader.get_entry();

    std::cout << " Loaded ELF: entry=0x" << std::hex << entry_point
              << std::endl;

    auto& memory = machine.get_memory();

    for (const auto& segment : reader.segments)
    {
        if (segment->get_type() == ELFIO::PT_LOAD &&
            segment->get_file_size() > 0)
        {
            uint32_t vaddr = segment->get_virtual_address();
            uint32_t size = segment->get_file_size();
            const uint8_t* data =
                reinterpret_cast<const uint8_t*>(segment->get_data());

            try
            {
                for (uint32_t i = 0; i < size; ++i)
                {
                    memory.write<uint8_t>(vaddr + i, data[i]);
                }
            }
            catch (const std::out_of_range& e)
            {
                std::cerr << " Memory overflow at 0x" << std::hex << vaddr
                          << ": " << e.what() << std::endl;
                return false;
            }

            std::cout << "   Segment: 0x" << vaddr << " size=" << std::dec
                      << size << " bytes" << std::endl;
        }
    }

    return true;
}
