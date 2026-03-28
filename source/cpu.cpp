#include <iostream>

#include "cpu.hpp"
#include "decoder.hpp"
#include "execute.hpp"
#include "memory.hpp"

CPU::CPU()
{
    reset();
}

void CPU::reset()
{
    state = CPUState();
}

uint32_t CPU::get_register(uint8_t index) const
{
    return state.gpr[index];
}

void CPU::set_register(uint8_t index, uint32_t value)
{
    if (index != 0)
    {
        state.gpr[index] = value;
    }
}

uint32_t CPU::get_pc() const
{
    return state.pc;
}

void CPU::set_pc(uint32_t value)
{
    state.pc = value;
}

bool CPU::is_halted() const
{
    return state.halt;
}

void CPU::step(Memory& memory)
{
    uint32_t raw_instr = memory.read<uint32_t>(state.pc);

    auto decoded_opt = Decoder::decode(raw_instr);
    if (!decoded_opt.has_value())
    {
        std::cerr << "Failed to decode instruction at PC=0x" << std::hex
                  << state.pc << std::endl;
        state.halt = true;
        return;
    }

    const DecodedInst& instr = decoded_opt.value();
    uint32_t old_pc = state.pc;

    Executor::execute(instr, state, memory);

    if (!state.halt && state.pc == old_pc)
    {
        state.pc += 4;
    }

#if CPU_DEBUG
    std::cout << "PC: 0x" << std::hex << old_pc << " -> 0x" << state.pc
              << " Instr: 0x" << raw_instr
              << " Type: " << Decoder::instr_type_to_string(instr.type)
              << std::endl;
#endif
}

void CPU::run(Memory& memory)
{
    std::cout << "Starting execution" << std::endl;
    int step_count = 0;

#if CPU_MAX_STEPS > 0
    const int max_steps = CPU_MAX_STEPS;
    while (!state.halt && step_count < max_steps)
    {
        step(memory);
        step_count++;
    }

    if (step_count >= max_steps)
    {
        std::cerr << "Reached step limit (" << max_steps << ")" << std::endl;
        state.halt = true;
    }
#else

    while (!state.halt)
    {
        step(memory);
        step_count++;
    }
#endif

    std::cout << "Program halted after " << step_count << " steps" << std::endl;
}

void CPU::run_with_limit(Memory& memory, uint32_t max_steps)
{
    uint32_t step_count = 0;
    while (!state.halt && step_count < max_steps)
    {
        step(memory);
        step_count++;
    }

    if (step_count >= max_steps)
    {
        std::cerr << "Reached manual step limit (" << max_steps << ")"
                  << std::endl;
    }
}
