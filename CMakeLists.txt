cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(moisture_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(moisture
	moisture.c
)

pico_enable_stdio_usb(moisture 1)
pico_enable_stdio_uart(moisture 1)

pico_add_extra_outputs(moisture)

target_link_libraries(moisture pico_stdlib)
target_link_libraries(moisture hardware_adc)
