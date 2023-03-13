#pragma once
// Premature implementation is the root of all wasted time

#if defined(_DEBUG) || !defined(NDEBUG)
	#define AL_DEBUG
#endif

#if defined(__GNUC__) && !defined(__clang__)
	#define AL_COMPILER_GNU
#elif defined(_MSC_VER)
	#define AL_COMPILER_MSVC
#elif defined(__clang__)
	#define AL_COMPILER_CLANG
#else
	#error Compiler not supported
#endif

#if defined(__linux__)
	#define AL_PLATFORM_LINUX
#elif defined(_WIN32) || defined(_WIN64)
	#define AL_PLATFORM_WINDOWS

	#if defined(__MINGW32__)
		#define AL_PLATFORM_WINDOWS_MINGW32
	#endif

	#if defined(__MINGW64__)
		#define AL_PLATFORM_WINDOWS_MINGW64
	#endif
#elif defined(__has_include) && __has_include(<pico/platform.h>)
	// TODO: use something other than __has_include
	// TODO: find a more reliable way to detect these boards

	#define AL_PLATFORM_PICO
	#define AL_PLATFORM_RP2040

	#if __has_include(<pico/cyw43_arch.h>)
		#define AL_PLATFORM_PICO_W
	#endif
#else
	#error Platform not supported
#endif

#if defined(__i386__) || defined(_M_IX86)
	#define      AL_X86
	#define AL_ARCH_X86
#elif defined(__x86_64__) || defined(_M_X64)
	#define      AL_X86_64
	#define AL_ARCH_X86_64
#elif defined(__arm__) || defined(_M_ARM)
	#define      AL_ARM
	#define AL_ARCH_ARM
#elif defined(__aarch64__)
	#define      AL_ARM64
	#define AL_ARCH_ARM64
#endif

#if defined(AL_COMPILER_GNU)
	#define AL_VECTORCALL

	#if defined(AL_X86)
		#define AL_CDECL      __attribute__((cdecl))
		#define AL_STDCALL    __attribute__((stdcall))
		#define AL_FASTCALL   __attribute__((fastcall))
		#define AL_THISCALL   __attribute__((thiscall))
	#elif defined(AL_X86_64)
		#define AL_CDECL
		#define AL_STDCALL    __attribute__((stdcall))
		#define AL_FASTCALL
		#define AL_THISCALL   __attribute__((thiscall))
	#else
		#define AL_CDECL
		#define AL_STDCALL
		#define AL_FASTCALL
		#define AL_THISCALL
	#endif

	#define AL_NAKED          __attribute__((naked))

	#define AL_INLINE         __attribute__((always_inline))
	#define AL_NO_INLINE      __attribute__((noinline))

	#define AL_DLL_EXPORT
	#define AL_DLL_IMPORT
#elif defined(AL_COMPILER_MSVC)
	#if defined(AL_X86)
		#define AL_CDECL      __cdecl
		#define AL_STDCALL    __stdcall
		#define AL_FASTCALL   __fastcall
		#define AL_THISCALL   __thiscall
		#define AL_VECTORCALL __vectorcall
	#elif defined(AL_X86_64)
		#define AL_CDECL
		#define AL_STDCALL    __stdcall
		#define AL_FASTCALL
		#define AL_THISCALL   __thiscall
		#define AL_VECTORCALL __vectorcall
	#else
		#define AL_CDECL
		#define AL_STDCALL
		#define AL_FASTCALL
		#define AL_THISCALL
		#define AL_VECTORCALL
	#endif

	#define AL_NAKED          __declspec(naked)

	#define AL_INLINE         __forceinline
	#define AL_NO_INLINE      __declspec(noinline)

	#if defined(AL_PLATFORM_WINDOWS)
		#define AL_DLL_EXPORT __declspec(dllexport)
		#define AL_DLL_IMPORT __declspec(dllimport)
	#else
		#define AL_DLL_EXPORT
		#define AL_DLL_IMPORT
	#endif
#elif defined(AL_COMPILER_CLANG)
	#define AL_CDECL

	#if defined(AL_X86)
		#define AL_STDCALL    __stdcall
		#define AL_FASTCALL   __fastcall
		#define AL_THISCALL   __thiscall
		#define AL_VECTORCALL __vectorcall
	#elif defined(AL_X86_64)
		#define AL_STDCALL
		#define AL_FASTCALL
		#define AL_THISCALL   __thiscall
		#define AL_VECTORCALL __vectorcall
	#else
		#define AL_STDCALL
		#define AL_FASTCALL
		#define AL_THISCALL
		#define AL_VECTORCALL
	#endif

	#define AL_NAKED          __declspec(naked)

	#define AL_INLINE         __forceinline
	#define AL_NO_INLINE      __declspec(noinline)

	#if defined(AL_PLATFORM_WINDOWS)
		#define AL_DLL_EXPORT __declspec(dllexport)
		#define AL_DLL_IMPORT __declspec(dllimport)
	#else
		#define AL_DLL_EXPORT
		#define AL_DLL_IMPORT
	#endif
#endif

#define AL_DEPRECATED(__text__) [[deprecated(__text__)]]

#define AL_STRINGIFY(__value__)  _AL_STRINGIFY(__value__)
#define _AL_STRINGIFY(__value__) #__value__

#define AL_CONCAT(__value1__, __value2__)  _AL_CONCAT(__value1__, __value2__)
#define _AL_CONCAT(__value1__, __value2__) __value1__##__value2__

#if defined(AL_DEBUG)
	#if defined(AL_COMPILER_GNU)
		#define AL_ASSERT(__condition__, __message__) _GLIBCXX_DEBUG_ASSERT((__condition__))
	#elif defined(AL_COMPILER_MSVC)
		#define AL_ASSERT(__condition__, __message__) _ASSERT_EXPR((__condition__), L##__message__)
	#elif defined(AL_COMPILER_CLANG)
		#define AL_ASSERT(__condition__, __message__) _GLIBCXX_DEBUG_ASSERT((__condition__))
	#endif
#else
	#define AL_ASSERT(__condition__, __message__)     ((void)0)
#endif

#if defined(__has_include)
	#define AL_HAS_INCLUDE                  __has_include
#else
	#define AL_HAS_INCLUDE(__file_path__) 0
#endif

#if defined(__has_feature)
	#define AL_HAS_FEATURE                  __has_feature
#else
	#define AL_HAS_FEATURE(__cpp_feature__) (__cpp_feature__ != 0)
#endif

#include "Common/Types.hpp"
#include "Common/Features.hpp"
#include "Common/TypeTraits.hpp"
#include "Common/Concepts.hpp"

#include "Common/Utility.hpp"

#include "Common/Endians.hpp"
#include "Common/Platforms.hpp"

#include "Common/Math.hpp"
#include "Common/BitMask.hpp"
#include "Common/BitConverter.hpp"

#include "Common/GUID.hpp"
#include "Common/String.hpp"
#include "Common/StringBuilder.hpp"

#include "Common/Exception.hpp"
#include "Common/NotImplementedException.hpp"
#include "Common/DependencyMissingException.hpp"
#include "Common/CompilerNotSupportedException.hpp"
#include "Common/PlatformNotSupportedException.hpp"
#include "Common/OperationNotSupportedException.hpp"

#include "Common/Regex.hpp"

#include "Common/DateTime.hpp"
#include "Common/Time.hpp"
#include "Common/TimeSpan.hpp"
#include "Common/Timestamp.hpp"

#include "Common/Function.hpp"

#include "Common/Event.hpp"
#include "Common/ScheduledEvent.hpp"

#include "Common/BitReader.hpp"
#include "Common/BaseConverter.hpp"
#include "Common/HexConverter.hpp"

#if defined(AL_PLATFORM_PICO)
	#include <pico.h>

	#include <pico/time.h> // sleep_us/busy_wait_us
	#include <pico/stdlib.h>

	#if !PICO_NO_FLASH
		#define AL_PICO_FLASH
	#else
		#define AL_PICO_NO_FLASH
	#endif
#elif defined(AL_PLATFORM_LINUX)
	#include <time.h> // timespec/timespec_get
	#include <unistd.h> // usleep
#elif defined(AL_PLATFORM_WINDOWS)
	#define VC_EXTRALEAN
	#define WIN32_LEAN_AND_MEAN

	#include <winapifamily.h>

	#if !defined(_WIN32_WINNT)
		#define _WIN32_WINNT_NT4          0x0400 // Windows NT 4.0
		#define _WIN32_WINNT_WIN2K        0x0500 // Windows 2000
		#define _WIN32_WINNT_WINXP        0x0501 // Windows XP
		#define _WIN32_WINNT_WS03         0x0502 // Windows Server 2003
		#define _WIN32_WINNT_WIN6         0x0600 // Windows Vista
		#define _WIN32_WINNT_VISTA        0x0600 // Windows Vista
		#define _WIN32_WINNT_WS08         0x0600 // Windows Server 2008
		#define _WIN32_WINNT_LONGHORN     0x0600 // Windows Vista
		#define _WIN32_WINNT_WIN7         0x0601 // Windows 7
		#define _WIN32_WINNT_WIN8         0x0602 // Windows 8
		#define _WIN32_WINNT_WINBLUE      0x0603 // Windows 8.1
		#define _WIN32_WINNT_WINTHRESHOLD 0x0A00 // Windows 10
		#define _WIN32_WINNT_WIN10        0x0A00 // Windows 10

		#if !defined(AL_WINDOWS_NT_XP) && !defined(AL_WINDOWS_NT_7) && !defined(AL_WINDOWS_NT_10)
			#define AL_WINDOWS_NT_7
		#endif

		#if defined(AL_WINDOWS_NT_XP)
			#define WINVER       _WIN32_WINNT_WINXP
			#define _WIN32_WINNT _WIN32_WINNT_WINXP
		#elif defined(AL_WINDOWS_NT_7)
			#define WINVER       _WIN32_WINNT_WIN7
			#define _WIN32_WINNT _WIN32_WINNT_WIN7
		#elif defined(AL_WINDOWS_NT_10)
			#define WINVER       _WIN32_WINNT_WIN10
			#define _WIN32_WINNT _WIN32_WINNT_WIN10
		#endif

		#define WINAPI_FAMILY    WINAPI_FAMILY_DESKTOP_APP
	#endif

	#include <Windows.h>

	// Generally these undefs go in the file that it impacts
	// - But GetMessage is so common that it's going here.
	#undef GetMessage
#endif

namespace AL
{
#if defined(AL_PLATFORM_RP2040)
	typedef int(Main)();
#elif defined(AL_PLATFORM_LINUX)
	typedef int(Main)(int argc, char* argv[]);
#elif defined(AL_PLATFORM_WINDOWS)
	typedef int(WINAPIV Main)(int argc, char* argv[], char* envp[]);
	typedef int(WINAPI WinMain)(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow);
	typedef int(WINAPI wWinMain)(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow);

	typedef BOOL(WINAPI DllMain)(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);
#endif

	inline Void Spin()
	{
#if defined(AL_PLATFORM_PICO)
		::busy_wait_us(0);
#elif defined(AL_PLATFORM_LINUX)
		// do nothing
#elif defined(AL_PLATFORM_WINDOWS)
		// do nothing
#endif
	}
	inline Void Spin(TimeSpan duration)
	{
#if defined(AL_PLATFORM_PICO)
		::busy_wait_us(
			duration.ToMicroseconds()
		);
#elif defined(AL_PLATFORM_LINUX)
		auto duration_Microseconds = duration.ToMicroseconds();

		uint64 start_Microseconds;
		uint64 elapsed_Microseconds;

		::timespec time;

		::timespec_get(
			&time,
			TIME_UTC
		);

		start_Microseconds = (time.tv_sec * 1000000) + (time.tv_nsec / 1000);

		do
		{
			::timespec_get(
				&time,
				TIME_UTC
			);

			elapsed_Microseconds = (time.tv_sec * 1000000) + (time.tv_nsec / 1000);
		} while ((elapsed_Microseconds - start_Microseconds) < duration_Microseconds);
#elif defined(AL_PLATFORM_WINDOWS)
		auto QueryPerformanceCounter = [](::LARGE_INTEGER& _integer, Double& _start, Double _frequency)
		{
			::QueryPerformanceCounter(
				&_integer
			);

			return TimeSpan::FromMicroseconds(
				static_cast<uint64>(
					(_integer.QuadPart - _start) / _frequency
				)
			);
		};

		auto QueryPerformanceFrequency = [](::LARGE_INTEGER& _integer)
		{
			::QueryPerformanceFrequency(
				&_integer
			);

			auto quad = static_cast<Double>(
				_integer.QuadPart
			);

			return quad / 1000000;
		};

		Double          start = 0;
		::LARGE_INTEGER integer = { .QuadPart = 0 };
		Double          frequency = QueryPerformanceFrequency(
			integer
		);

		while (QueryPerformanceCounter(integer, start, frequency) < duration)
		{
		}
#endif
	}

	inline Void Sleep()
	{
#if defined(AL_PLATFORM_PICO)
		::sleep_us(
			0
		);
#elif defined(AL_PLATFORM_LINUX)
		::usleep(
			0
		);
#elif defined(AL_PLATFORM_WINDOWS)
		::Sleep(
			0
		);
#endif
	}
	inline Void Sleep(TimeSpan duration)
	{
#if defined(AL_PLATFORM_PICO)
		::sleep_us(
			duration.ToMicroseconds()
		);
#elif defined(AL_PLATFORM_LINUX)
		::usleep(
			static_cast<::__useconds_t>(duration.ToMicroseconds())
		);
#elif defined(AL_PLATFORM_WINDOWS)
		::Sleep(
			static_cast<::DWORD>(duration.ToMilliseconds())
		);
#endif
	}
}
