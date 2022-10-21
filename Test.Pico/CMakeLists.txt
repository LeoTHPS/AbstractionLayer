cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)
set(PICO_BOARD pico)
set(PICO_CXX_ENABLE_RTTI 1)
set(PICO_CXX_ENABLE_EXCEPTIONS 1)

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
	hardware_adc
	hardware_i2c
	hardware_spi
	hardware_gpio
	hardware_uart
	hardware_timer
)

target_include_directories(
	Test_Pico
	PRIVATE
	"."
	".."
)

pico_add_extra_outputs(
	Test_Pico
)

pico_enable_stdio_usb(
	Test_Pico
	1
)

pico_enable_stdio_uart(
	Test_Pico
	0
)
