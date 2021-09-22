cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(moisture_project)
pico_sdk_init()

add_executable(moisture)

pico_generate_pio_header(moisture
	${CMAKE_CURRENT_LIST_DIR}/ws2812.pio
)

target_sources(moisture
	PRIVATE
	moisture.c
)

pico_enable_stdio_usb(moisture 0)
pico_enable_stdio_uart(moisture 1)

pico_add_extra_outputs(moisture)

target_link_libraries(moisture
	PRIVATE
	pico_stdlib
	hardware_adc
	hardware_pio
)
