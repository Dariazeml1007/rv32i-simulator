cmake_minimum_required(VERSION 3.16)
project(RISCV_SIMULATOR LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


option(RUN_TESTS "Build test executable" OFF)
option(CPU_DEBUG "Enable CPU debug output" OFF)
option(CPU_STEP_LIMIT "Enable CPU step limit (0 = unlimited)" ON)


set(CPU_MAX_STEPS "10000" CACHE STRING "Maximum number of steps before halt (0 = unlimited)")


set(COMMON_COMPILE_OPTIONS
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall>
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wextra>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

set(COMMON_DEFINITIONS
    $<$<BOOL:${CPU_DEBUG}>:CPU_DEBUG=1>
)

if(CPU_STEP_LIMIT AND NOT CPU_MAX_STEPS STREQUAL "0")
    list(APPEND COMMON_DEFINITIONS CPU_MAX_STEPS=${CPU_MAX_STEPS})
else()
    list(APPEND COMMON_DEFINITIONS CPU_MAX_STEPS=0)
endif()


# Проверяем, существует ли папка с библиотекой
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/elfio")
    add_library(elfio INTERFACE)
    target_include_directories(elfio INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/elfio
    )
    set(ELFIO_FOUND ON)
    message(STATUS " ELFIO library found")
else()
    set(ELFIO_FOUND OFF)
    message(STATUS " ELFIO library NOT found - ELF loading disabled")
endif()


find_program(RISCV_AS riscv32-unknown-elf-as)
find_program(RISCV_LD riscv32-unknown-elf-ld)
find_program(RISCV_OBJCOPY riscv32-unknown-elf-objcopy)

if(RISCV_AS AND RISCV_LD)
    message(STATUS "RISC-V toolchain found: ${RISCV_AS}")
    set(HAS_RISCV_TOOLCHAIN ON)
else()
    message(STATUS " RISC-V toolchain NOT found - ELF tests will be skipped")
    set(HAS_RISCV_TOOLCHAIN OFF)
endif()


set(EMULATOR_SOURCES
    source/cpu.cpp
    source/decoder.cpp
    source/execute.cpp
    source/machine.cpp
    source/elf_loader.cpp
    source/main.cpp
)

add_executable(cpu_emulator ${EMULATOR_SOURCES})
target_include_directories(cpu_emulator PRIVATE include)
target_compile_options(cpu_emulator PRIVATE ${COMMON_COMPILE_OPTIONS})
target_compile_definitions(cpu_emulator PRIVATE ${COMMON_DEFINITIONS})

# Подключаем elfio, если найден
if(ELFIO_FOUND)
    target_link_libraries(cpu_emulator PRIVATE elfio)
    target_compile_definitions(cpu_emulator PRIVATE ELFIO_ENABLED=1)
endif()


if(RUN_TESTS)
    set(TEST_SOURCES
        source/cpu.cpp
        source/decoder.cpp
        source/execute.cpp
        source/machine.cpp
        tests/test.cpp
    )

    add_executable(cpu_emulator_tests ${TEST_SOURCES})
    target_include_directories(cpu_emulator_tests PRIVATE include)
    target_compile_options(cpu_emulator_tests PRIVATE ${COMMON_COMPILE_OPTIONS})
    target_compile_definitions(cpu_emulator_tests PRIVATE
        ${COMMON_DEFINITIONS}
        RUN_TESTS=1
    )

    if(ELFIO_FOUND)
        target_link_libraries(cpu_emulator_tests PRIVATE elfio)
    endif()

    message(STATUS "Building test executable: cpu_emulator_tests")
endif()


message(STATUS "========================================")
message(STATUS "Build configuration:")
message(STATUS "  CPU_DEBUG: ${CPU_DEBUG}")
message(STATUS "  CPU_STEP_LIMIT: ${CPU_STEP_LIMIT}")
message(STATUS "  CPU_MAX_STEPS: ${CPU_MAX_STEPS}")
message(STATUS "  RUN_TESTS: ${RUN_TESTS}")
message(STATUS "  ELFIO_FOUND: ${ELFIO_FOUND}")
message(STATUS "  HAS_RISCV_TOOLCHAIN: ${HAS_RISCV_TOOLCHAIN}")
message(STATUS "========================================")
