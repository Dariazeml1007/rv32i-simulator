cmake_minimum_required(VERSION 3.16)
project(RISCV_SIMULATOR LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================
# Пользовательские опции (можно менять при сборке)
# ============================================
option(RUN_TESTS "Build test executable" OFF)
option(CPU_DEBUG "Enable CPU debug output" OFF)
option(CPU_STEP_LIMIT "Enable CPU step limit (0 = unlimited)" ON)

# ============================================
# Настройки лимита шагов
# ============================================
set(CPU_MAX_STEPS "10000" CACHE STRING "Maximum number of steps before halt (0 = unlimited)")

# ============================================
# Общие настройки компиляции
# ============================================
set(COMMON_COMPILE_OPTIONS
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall>
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wextra>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# ============================================
# Определения препроцессора
# ============================================
set(COMMON_DEFINITIONS
    $<$<BOOL:${CPU_DEBUG}>:CPU_DEBUG=1>
)

# Устанавливаем лимит шагов
if(CPU_STEP_LIMIT AND NOT CPU_MAX_STEPS STREQUAL "0")
    list(APPEND COMMON_DEFINITIONS CPU_MAX_STEPS=${CPU_MAX_STEPS})
else()
    list(APPEND COMMON_DEFINITIONS CPU_MAX_STEPS=0)
endif()

# ============================================
# Основной эмулятор
# ============================================
set(EMULATOR_SOURCES
    source/cpu.cpp
    source/decoder.cpp
    source/execute.cpp
    source/machine.cpp
    source/main.cpp
)

add_executable(cpu_emulator ${EMULATOR_SOURCES})
target_include_directories(cpu_emulator PRIVATE include)
target_compile_options(cpu_emulator PRIVATE ${COMMON_COMPILE_OPTIONS})
target_compile_definitions(cpu_emulator PRIVATE ${COMMON_DEFINITIONS})

# ============================================
# Тестовый исполняемый файл
# ============================================
if(RUN_TESTS)
    set(TEST_SOURCES
        source/cpu.cpp
        source/decoder.cpp
        source/executor.cpp
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
    message(STATUS "Building test executable: cpu_emulator_tests")
endif()

# ============================================
# Информация о сборке
# ============================================
message(STATUS "========================================")
message(STATUS "Build configuration:")
message(STATUS "  CPU_DEBUG: ${CPU_DEBUG}")
message(STATUS "  CPU_STEP_LIMIT: ${CPU_STEP_LIMIT}")
message(STATUS "  CPU_MAX_STEPS: ${CPU_MAX_STEPS}")
message(STATUS "  RUN_TESTS: ${RUN_TESTS}")
message(STATUS "========================================")
