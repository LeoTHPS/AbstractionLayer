cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20)

set(PICO_BOARD pico_w)
set(PICO_CXX_ENABLE_RTTI 1)
set(PICO_CXX_ENABLE_EXCEPTIONS 1)

include(pico_sdk_import.cmake)

project(Test_PicoW)

pico_sdk_init()

add_executable(
	Test_PicoW
	main.cpp
)

target_link_libraries(
	Test_PicoW
	pico_stdlib
	pico_malloc
	pico_multicore
	pico_lwip
	pico_cyw43_arch_lwip_poll
#[[	pico_cyw43_arch_lwip_threadsafe_background ]]
	hardware_adc
	hardware_i2c
	hardware_spi
	hardware_gpio
	hardware_uart
	hardware_timer
)

target_include_directories(
	Test_PicoW
	PRIVATE
	"."
	".."
)

target_compile_definitions(
	Test_PicoW
	PRIVATE
	AL_PLATFORM_PICO_W
)

pico_add_extra_outputs(
	Test_PicoW
)

pico_enable_stdio_usb(
	Test_PicoW
	1
)

pico_enable_stdio_uart(
	Test_PicoW
	0
)
