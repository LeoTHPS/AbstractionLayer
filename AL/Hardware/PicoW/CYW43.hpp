#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_PICO_W)
	#warning Platform not supported
#endif

#include "AL/OS/SystemException.hpp"

#if AL_HAS_INCLUDE(<pico/cyw43_arch.h>)
	#define AL_DEPENDENCY_CYW43

	#include <pico/cyw43_arch.h>

	#include <boards/pico_w.h>

	#if defined(CYW43_LWIP)
		#define AL_DEPENDENCY_CYW43_LWIP CYW43_LWIP
	#endif

	#if defined(PICO_CYW43_ARCH_THREADSAFE_BACKGROUND)
		#define AL_DEPENDENCY_PICO_CYW43_ARCH_THREADSAFE_BACKGROUND
	#endif
#endif

namespace AL::Hardware::PicoW
{
	enum class CYW43AuthTypes : uint32
	{
		Open           = CYW43_AUTH_OPEN,
		WPA_TKIP       = CYW43_AUTH_WPA_TKIP_PSK,
		WPA2_AES       = CYW43_AUTH_WPA2_AES_PSK,
		WPA2_MIXED_PSK = CYW43_AUTH_WPA2_MIXED_PSK
	};

	class CYW43
	{
		inline static Bool isOpen      = False;
		inline static Bool isConnected = False;
		inline static Bool isListening = False;

		CYW43() = delete;

	public:
		class LED
		{
			LED() = delete;

		public:
			// @throw AL::Exception
			static Void Read(Bool& value)
			{
				AL_ASSERT(
					IsOpen(),
					"CYW43 not open"
				);

				value = ::cyw43_arch_gpio_get(
					CYW43_WL_GPIO_LED_PIN
				);
			}

			// @throw AL::Exception
			static Void Write(Bool value)
			{
				AL_ASSERT(
					IsOpen(),
					"CYW43 not open"
				);

				::cyw43_arch_gpio_put(
					CYW43_WL_GPIO_LED_PIN,
					value
				);
			}
		};

		static Bool IsOpen()
		{
			return isOpen;
		}

		static Bool IsConnected()
		{
			return isConnected;
		}

		static Bool IsListening()
		{
			return isListening;
		}

		// @throw AL::Exception
		static Void Open()
		{
			AL_ASSERT(
				!IsOpen(),
				"CYW43 already open"
			);

#if defined(AL_DEPENDENCY_CYW43)
			::cyw43_arch_init();
#else
			throw DependencyMissingException(
				"CYW43"
			);
#endif

			isOpen = True;
		}

		static Void Close()
		{
			if (IsOpen())
			{
#if defined(AL_DEPENDENCY_CYW43)
				::cyw43_arch_deinit();
#endif

				isOpen      = False;
				isConnected = False;
				isListening = False;
			}
		}

		// TODO: implement
		// @throw AL::Exception
		static Void Scan();

		// @throw AL::Exception
		static Void Connect(const String& ssid)
		{
			AL_ASSERT(
				IsOpen(),
				"CYW43 not open"
			);

#if defined(AL_DEPENDENCY_CYW43)
			OS::ErrorCode errorCode;

			if ((errorCode = ::cyw43_arch_wifi_connect_blocking(ssid.GetCString(), nullptr, CYW43_AUTH_OPEN)) != PICO_ERROR_NONE)
			{

				throw OS::SystemException(
					"cyw43_arch_wifi_connect_blocking",
					errorCode
				);
			}
#else
			throw NotImplementedException();
#endif

			isConnected = True;
			isListening = False;
		}
		// @throw AL::Exception
		static Void Connect(const String& ssid, const String& password, CYW43AuthTypes authType)
		{
			AL_ASSERT(
				IsOpen(),
				"CYW43 not open"
			);

#if defined(AL_DEPENDENCY_CYW43)
			OS::ErrorCode errorCode;

			if ((errorCode = ::cyw43_arch_wifi_connect_blocking(ssid.GetCString(), nullptr, CYW43_AUTH_OPEN)) != PICO_ERROR_NONE)
			{

				throw OS::SystemException(
					"cyw43_arch_wifi_connect_blocking",
					errorCode
				);
			}
#else
			throw NotImplementedException();
#endif

			isConnected = True;
			isListening = False;
		}

		// @throw AL::Exception
		static Void Listen(const String& ssid)
		{
			AL_ASSERT(
				IsOpen(),
				"CYW43 not open"
			);

#if defined(AL_DEPENDENCY_CYW43)
			::cyw43_arch_enable_ap_mode(
				ssid.GetCString(),
				nullptr,
				CYW43_AUTH_OPEN
			);
#else
			throw NotImplementedException();
#endif

			isConnected = False;
			isListening = True;
		}
		// @throw AL::Exception
		static Void Listen(const String& ssid, const String& password, CYW43AuthTypes authType)
		{
			AL_ASSERT(
				IsOpen(),
				"CYW43 not open"
			);

#if defined(AL_DEPENDENCY_CYW43)
			::cyw43_arch_enable_ap_mode(
				ssid.GetCString(),
				password.GetCString(),
				static_cast<uint32_t>(authType)
			);
#else
			throw NotImplementedException();
#endif

			isConnected = False;
			isListening = True;
		}
	};
}
