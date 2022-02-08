cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)

include(pico_sdk_import.cmake)

project(Test_Pico)

pico_sdk_init()

add_executable(
	Test_Pico
	main.cpp
)

target_link_libraries(
	Test_Pico
	pico_stdlib
	pico_malloc
	pico_multicore
	hardware_i2c
	hardware_spi
	hardware_gpio
	hardware_uart
	hardware_timer
)

target_include_directories(
	Test_Pico
	PRIVATE
	"../"
)

target_compile_definitions(
	Test_Pico
	PRIVATE
	AL_PLATFORM_PICO
)

pico_add_extra_outputs(
	Test_Pico
)

pico_enable_stdio_usb(
	Test_Pico
	0
)

pico_enable_stdio_uart(
	Test_Pico
	1
)
