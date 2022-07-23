#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_PICO_W)
	#warning Platform not supported
#endif

#include "AL/OS/SystemException.hpp"

#if AL_HAS_INCLUDE(<pico/cyw43_arch.h>)
	#define AL_DEPENDENCY_PICO_CYW43

	#include <pico/cyw43_arch.h>

	#include <boards/pico_w.h>

	#if defined(CYW43_LWIP)
		#define AL_DEPENDENCY_PICO_CYW43_LWIP CYW43_LWIP

		#if defined(LWIP_IPV4) && LWIP_IPV4
			#define AL_DEPENDENCY_PICO_CYW43_LWIP_IPV4
		#endif

		#if defined(LWIP_IPV6) && LWIP_IPV6
			#define AL_DEPENDENCY_PICO_CYW43_LWIP_IPV6
		#endif
	#endif

	#if defined(PICO_CYW43_ARCH_POLL)
		#define AL_DEPENDENCY_PICO_CYW43_ARCH_POLL
	#elif defined(PICO_CYW43_ARCH_THREADSAFE_BACKGROUND)
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

	enum class CYW43Countries : uint32
	{
		Worldwide      = CYW43_COUNTRY_WORLDWIDE,

		Australia      = CYW43_COUNTRY_AUSTRALIA,
		Austria        = CYW43_COUNTRY_AUSTRIA,
		Belgium        = CYW43_COUNTRY_BELGIUM,
		Brazil         = CYW43_COUNTRY_BRAZIL,
		Canada         = CYW43_COUNTRY_CANADA,
		Chile          = CYW43_COUNTRY_CHILE,
		China          = CYW43_COUNTRY_CHINA,
		Colombia       = CYW43_COUNTRY_COLOMBIA,
		Czech_Republic = CYW43_COUNTRY_CZECH_REPUBLIC,
		Denmark        = CYW43_COUNTRY_DENMARK,
		Estonia        = CYW43_COUNTRY_ESTONIA,
		Finland        = CYW43_COUNTRY_FINLAND,
		France         = CYW43_COUNTRY_FRANCE,
		Germany        = CYW43_COUNTRY_GERMANY,
		Greece         = CYW43_COUNTRY_GREECE,
		Hong_Kong      = CYW43_COUNTRY_HONG_KONG,
		Hungary        = CYW43_COUNTRY_HUNGARY,
		Iceland        = CYW43_COUNTRY_ICELAND,
		India          = CYW43_COUNTRY_INDIA,
		Israel         = CYW43_COUNTRY_ISRAEL,
		Italy          = CYW43_COUNTRY_ITALY,
		Japan          = CYW43_COUNTRY_JAPAN,
		Kenya          = CYW43_COUNTRY_KENYA,
		Latvia         = CYW43_COUNTRY_LATVIA,
		Liechtenstein  = CYW43_COUNTRY_LIECHTENSTEIN,
		Lithuania      = CYW43_COUNTRY_LITHUANIA,
		Luxembourg     = CYW43_COUNTRY_LUXEMBOURG,
		Malaysia       = CYW43_COUNTRY_MALAYSIA,
		Malta          = CYW43_COUNTRY_MALTA,
		Mexico         = CYW43_COUNTRY_MEXICO,
		Netherlands    = CYW43_COUNTRY_NETHERLANDS,
		New_Zealand    = CYW43_COUNTRY_NEW_ZEALAND,
		Nigeria        = CYW43_COUNTRY_NIGERIA,
		Norway         = CYW43_COUNTRY_NORWAY,
		Peru           = CYW43_COUNTRY_PERU,
		Philippines    = CYW43_COUNTRY_PHILIPPINES,
		Poland         = CYW43_COUNTRY_POLAND,
		Portugal       = CYW43_COUNTRY_PORTUGAL,
		Singapore      = CYW43_COUNTRY_SINGAPORE,
		Slovakia       = CYW43_COUNTRY_SLOVAKIA,
		Slovenia       = CYW43_COUNTRY_SLOVENIA,
		South_Africa   = CYW43_COUNTRY_SOUTH_AFRICA,
		South_Korea    = CYW43_COUNTRY_SOUTH_KOREA,
		Spain          = CYW43_COUNTRY_SPAIN,
		Sweden         = CYW43_COUNTRY_SWEDEN,
		Switzerland    = CYW43_COUNTRY_SWITZERLAND,
		Taiwan         = CYW43_COUNTRY_TAIWAN,
		Thailand       = CYW43_COUNTRY_THAILAND,
		Turkey         = CYW43_COUNTRY_TURKEY,
		UK             = CYW43_COUNTRY_UK,
		USA            = CYW43_COUNTRY_USA
	};

	struct CYW43Network
	{
		int16          RSSI;
		String         SSID;
		String         BSSID;
		uint16         Channel;
		CYW43AuthTypes AuthType;
	};

	// @throw AL::Exception
	// @return AL::False to stop scanning
	typedef Function<Bool(const CYW43Network& network)> CYW43ScanCallback;

	class CYW43
	{
		struct ScanContext
		{
			Bool                     Stop;
			const CYW43ScanCallback* lpCallback;
		};

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

		static Void Poll()
		{
			AL_ASSERT(
				IsOpen(),
				"CYW43 not open"
			);

#if defined(PICO_CYW43_ARCH_POLL)
			::cyw43_arch_poll();
#elif defined(PICO_CYW43_ARCH_THREADSAFE_BACKGROUND)
			Spin();
#endif
		}

		// @throw AL::Exception
		static Void Open()
		{
			AL_ASSERT(
				!IsOpen(),
				"CYW43 already open"
			);

			Open(
				CYW43Countries::Worldwide
			);
		}
		// @throw AL::Exception
		static Void Open(CYW43Countries country)
		{
			AL_ASSERT(
				!IsOpen(),
				"CYW43 already open"
			);

#if defined(AL_DEPENDENCY_PICO_CYW43)
			OS::ErrorCode errorCode;

			if ((errorCode = ::cyw43_arch_init_with_country(static_cast<typename Get_Enum_Or_Integer_Base<CYW43Countries>::Type>(country))) != PICO_ERROR_NONE)
			{

				throw OS::SystemException(
					"cyw43_arch_init_with_country",
					errorCode
				);
			}

			try
			{
				LED::Write(
					False
				);
			}
			catch (Exception& exception)
			{
				::cyw43_arch_deinit();

				throw Exception(
					Move(exception),
					"Error disabling LED"
				);
			}
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
#if defined(AL_DEPENDENCY_PICO_CYW43)
				try
				{
					LED::Write(
						False
					);
				}
				catch (const Exception&)
				{
				}

				::cyw43_arch_deinit();
#endif

				isOpen      = False;
				isConnected = False;
				isListening = False;
			}
		}

		// @throw AL::Exception
		static Void Scan(const CYW43ScanCallback& callback)
		{
			AL_ASSERT(
				!IsOpen(),
				"CYW43 already open"
			);

#if defined(AL_DEPENDENCY_PICO_CYW43)
			OS::ErrorCode errorCode;

			ScanContext context =
			{
				.Stop       = False,
				.lpCallback = &callback
			};

			::cyw43_wifi_scan_options_t options =
			{
				.ssid_len  = 0, // 0=all
				.scan_type = 0  // 0=active, 1=passive
			};

			cyw43_arch_enable_sta_mode();

			if ((errorCode = ::cyw43_wifi_scan(&cyw43_state, &options, &context, &OnScan)) != PICO_ERROR_NONE)
			{

				throw OS::SystemException(
					"cyw43_wifi_scan",
					errorCode
				);
			}

			auto timeSpan = TimeSpan::FromMilliseconds(
				100
			);

			while (::cyw43_wifi_scan_active(&cyw43_state))
			{
				Poll();
			}
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		static Void Connect(const String& ssid)
		{
			AL_ASSERT(
				IsOpen(),
				"CYW43 not open"
			);

#if defined(AL_DEPENDENCY_PICO_CYW43)
			OS::ErrorCode errorCode;

			::cyw43_arch_enable_sta_mode();

			if ((errorCode = ::cyw43_arch_wifi_connect_blocking(ssid.GetCString(), nullptr, CYW43_AUTH_OPEN)) != PICO_ERROR_NONE)
			{

				throw OS::SystemException(
					"cyw43_arch_wifi_connect_timeout_ms",
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

#if defined(AL_DEPENDENCY_PICO_CYW43)
			OS::ErrorCode errorCode;

			::cyw43_arch_enable_sta_mode();

			if ((errorCode = ::cyw43_arch_wifi_connect_blocking(ssid.GetCString(), password.GetCString(), static_cast<typename Get_Enum_Or_Integer_Base<CYW43AuthTypes>::Type>(authType))) != PICO_ERROR_NONE)
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
		// @return AL::False on timeout
		static Bool TryConnect(const String& ssid, TimeSpan timeout = TimeSpan::Infinite)
		{
			AL_ASSERT(
				IsOpen(),
				"CYW43 not open"
			);

#if defined(AL_DEPENDENCY_PICO_CYW43)
			OS::ErrorCode errorCode;

			::cyw43_arch_enable_sta_mode();

			if ((errorCode = ::cyw43_arch_wifi_connect_timeout_ms(ssid.GetCString(), nullptr, CYW43_AUTH_OPEN, static_cast<::uint32_t>(timeout.ToMilliseconds()))) != PICO_ERROR_NONE)
			{
				if (errorCode == PICO_ERROR_TIMEOUT)
				{

					return False;
				}

				throw OS::SystemException(
					"cyw43_arch_wifi_connect_timeout_ms",
					errorCode
				);
			}
#else
			throw NotImplementedException();
#endif

			isConnected = True;
			isListening = False;

			return True;
		}
		// @throw AL::Exception
		// @return AL::False on timeout
		static Bool TryConnect(const String& ssid, const String& password, CYW43AuthTypes authType, TimeSpan timeout = TimeSpan::Infinite)
		{
			AL_ASSERT(
				IsOpen(),
				"CYW43 not open"
			);

#if defined(AL_DEPENDENCY_PICO_CYW43)
			OS::ErrorCode errorCode;

			::cyw43_arch_enable_sta_mode();

			if ((errorCode = ::cyw43_arch_wifi_connect_timeout_ms(ssid.GetCString(), password.GetCString(), static_cast<typename Get_Enum_Or_Integer_Base<CYW43AuthTypes>::Type>(authType), static_cast<::uint32_t>(timeout.ToMilliseconds()))) != PICO_ERROR_NONE)
			{
				if (errorCode == PICO_ERROR_TIMEOUT)
				{

					return False;
				}

				throw OS::SystemException(
					"cyw43_arch_wifi_connect_timeout_ms",
					errorCode
				);
			}
#else
			throw NotImplementedException();
#endif

			isConnected = True;
			isListening = False;

			return True;
		}

		// @throw AL::Exception
		static Void Listen(const String& ssid)
		{
			AL_ASSERT(
				IsOpen(),
				"CYW43 not open"
			);

#if defined(AL_DEPENDENCY_PICO_CYW43)
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

#if defined(AL_DEPENDENCY_PICO_CYW43)
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

	private:
		static int OnScan(void* lpParam, const cyw43_ev_scan_result_t* lpResult)
		{
			if (auto lpContext = reinterpret_cast<ScanContext*>(lpParam); !lpContext->Stop)
			{
				CYW43Network network =
				{
					.RSSI     = lpResult->rssi,
					.SSID     = String(reinterpret_cast<const char*>(lpResult->ssid), lpResult->ssid_len),
					.BSSID    = BSSID_ToString(lpResult->bssid),
					.Channel  = lpResult->channel,
					.AuthType = static_cast<CYW43AuthTypes>(lpResult->auth_mode)
				};

				if (!(*lpContext->lpCallback)(network))
				{

					lpContext->Stop = True;
				}
			}

			return 0;
		}

		static String BSSID_ToString(const ::uint8_t(&value)[6])
		{
			return String::Format(
				"%02X:%02X:%02X:%02X:%02X:%02X",
				value[0],
				value[1],
				value[2],
				value[3],
				value[4],
				value[5]
			);
		}
	};
}
