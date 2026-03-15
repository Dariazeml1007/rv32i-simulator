#ifndef CPU_HPP
#define CPU_HPP

#include <cstdint>
#include <array>
#include "types.hpp"


#ifndef CPU_DEBUG
#define CPU_DEBUG 0  
#endif

#ifndef CPU_MAX_STEPS
#define CPU_MAX_STEPS 10000  
#endif


class CPU {
private:
    CPUState state;

public:
    CPU();

    void reset();
    void step(class Memory& memory);
    void run(class Memory& memory);

    
    uint32_t get_register(uint8_t index) const;
    void set_register(uint8_t index, uint32_t value);
    uint32_t get_pc() const;
    void set_pc(uint32_t value);
    bool is_halted() const;

    
    const CPUState& get_state() const { return state; }
    CPUState& get_state() { return state; }

    
    void run_with_limit(Memory& memory, uint32_t max_steps);
};

#endif // CPU_HPP
