cmake_minimum_required(VERSION 3.13)

project(stm32l4-adc-dma-signal-processing LANGUAGES C CXX ASM)

set(MCU_TARGET STM32L476RG)
set(CPU_FAMILY STM32L4)

# Source files
set(SOURCES
    Src/main.cpp
    Src/system_clock.c
)

# Header search paths
include_directories(
    Inc
    Drivers/STM32L4xx_HAL_Driver/Inc
    Drivers/CMSIS/Device/ST/STM32L4xx/Include
    Drivers/CMSIS/Include
)

add_executable(${PROJECT_NAME}.elf ${SOURCES})

# Linker script (adjust path as needed)
target_link_options(${PROJECT_NAME}.elf PRIVATE
    -T${CMAKE_SOURCE_DIR}/STM32L476RGTx_FLASH.ld
)

# Compiler flags
target_compile_options(${PROJECT_NAME}.elf PRIVATE
    -std=gnu++17
    -Wall -Wextra
)

# Post-build: generate .bin and .hex
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Obinary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -Oihex  ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
)
