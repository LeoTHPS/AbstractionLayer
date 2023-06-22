#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_PICO_W)
	#error Platform not supported
#endif

#include "AL/OS/SystemException.hpp"

#if !AL_HAS_INCLUDE(<pico/cyw43_arch.h>)
	#error Missing pico/cyw43_arch.h
#endif

#include <boards/pico_w.h>

#include <pico/cyw43_arch.h>

#if defined(LWIP_IPV4) && LWIP_IPV4
	#define AL_DEPENDENCY_PICO_CYW43_LWIP_IPV4
#endif

#if defined(LWIP_IPV6) && LWIP_IPV6
	#define AL_DEPENDENCY_PICO_CYW43_LWIP_IPV6
#endif

#if defined(PICO_CYW43_ARCH_POLL)
	#define AL_DEPENDENCY_PICO_CYW43_ARCH_POLL
#elif defined(PICO_CYW43_ARCH_THREADSAFE_BACKGROUND)
	#define AL_DEPENDENCY_PICO_CYW43_ARCH_THREADSAFE_BACKGROUND
#endif

namespace AL::Hardware::PicoW
{
	enum class CYW43AuthTypes : uint8
	{
		Open,

		WEP_PSK,

		WPA_TKIP_PSK,

		WPA2_AES_PSK,
		WPA2_MIXED_PSK
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

	enum class CYW43LinkStatus : int8
	{
		// Wifi down
		Down    = CYW43_LINK_DOWN,
		// Connected to wifi
		Join    = CYW43_LINK_JOIN,
		// Connection failed
		Fail    = CYW43_LINK_FAIL,
		// No matching SSID found (could be out of range, or down)
		NoNet   = CYW43_LINK_NONET,
		// Authenticatation failure
		BadAuth = CYW43_LINK_BADAUTH
	};

	enum class CYW43PowerLevels : uint8
	{
		Default,
		Aggressive,
		Performance
	};

	struct CYW43Network
	{
		int16          RSSI;
		String         SSID;
		String         BSSID;
		uint16         Channel;
		CYW43AuthTypes AuthType;
	};

	typedef Collections::Array<uint8[6]> CYW43MACAddress;

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

		inline static Bool isOpen        = False;
		inline static Bool isStation     = False;
		inline static Bool isAccessPoint = False;
		inline static Bool isConnected   = False;
		inline static Bool isListening   = False;

		CYW43() = delete;

	public:
		typedef CYW43Network     Network;
		typedef CYW43AuthTypes   AuthTypes;
		typedef CYW43Countries   Countries;
		typedef CYW43LinkStatus  LinkStatus;
		typedef CYW43MACAddress  MACAddress;
		typedef CYW43PowerLevels PowerLevels;

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

		static Bool IsStation()
		{
			return isStation;
		}

		static Bool IsAccessPoint()
		{
			return isAccessPoint;
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
		static int32 GetRSSI()
		{
			AL_ASSERT(
				IsOpen(),
				"CYW43 not open"
			);

			::int32_t     value;
			OS::ErrorCode errorCode;

			if ((errorCode = ::cyw43_wifi_get_rssi(&cyw43_state, &value)) != 0)
			{

				throw OS::SystemException(
					"cyw43_wifi_get_rssi",
					errorCode
				);
			}

			return value;
		}

		// @throw AL::Exception
		static LinkStatus GetLinkStatus()
		{
			AL_ASSERT(
				IsOpen(),
				"CYW43 not open"
			);

			int value;

			if ((value = ::cyw43_wifi_link_status(&cyw43_state, IsStation() ? ::CYW43_ITF_STA : ::CYW43_ITF_AP)) < 0)
			{

				throw OS::SystemException(
					"cyw43_wifi_link_status",
					value
				);
			}

			return static_cast<LinkStatus>(
				value
			);
		}

		// @throw AL::Exception
		static MACAddress GetMACAddress()
		{
			AL_ASSERT(
				IsOpen(),
				"CYW43 not open"
			);

			::uint8_t     value[6];
			OS::ErrorCode errorCode;

			if ((errorCode = ::cyw43_wifi_get_mac(&cyw43_state, IsStation() ? ::CYW43_ITF_STA : ::CYW43_ITF_AP, value)) != 0)
			{

				throw OS::SystemException(
					"cyw43_wifi_get_mac",
					errorCode
				);
			}

			return value;
		}

		// @throw AL::Exception
		static PowerLevels GetPowerManagement()
		{
			AL_ASSERT(
				IsOpen(),
				"CYW43 not open"
			);

			::uint32_t    value;
			OS::ErrorCode errorCode;

			if ((errorCode = ::cyw43_wifi_get_pm(&cyw43_state, &value)) != 0)
			{

				throw OS::SystemException(
					"cyw43_wifi_get_pm",
					errorCode
				);
			}

			if (value == CYW43_DEFAULT_PM)
			{

				return PowerLevels::Default;
			}

			if (value == CYW43_AGGRESSIVE_PM)
			{

				return PowerLevels::Aggressive;
			}

			if (value == CYW43_PERFORMANCE_PM)
			{

				return PowerLevels::Performance;
			}

			throw NotImplementedException();
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
				Countries::Worldwide
			);
		}
		// @throw AL::Exception
		static Void Open(Countries country)
		{
			AL_ASSERT(
				!IsOpen(),
				"CYW43 already open"
			);

			OS::ErrorCode errorCode;

			if ((errorCode = ::cyw43_arch_init_with_country(static_cast<typename Get_Enum_Or_Integer_Base<Countries>::Type>(country))) != PICO_ERROR_NONE)
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

			isOpen = True;
		}

		static Void Close()
		{
			if (IsOpen())
			{
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

				isOpen        = False;
				isConnected   = False;
				isListening   = False;
				isStation     = False;
				isAccessPoint = False;
			}
		}

		// @throw AL::Exception
		static Void Scan(const CYW43ScanCallback& callback)
		{
			AL_ASSERT(
				!IsOpen(),
				"CYW43 already open"
			);

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
		}

		// @throw AL::Exception
		static Void Connect(const String& ssid)
		{
			AL_ASSERT(
				IsOpen(),
				"CYW43 not open"
			);

			OS::ErrorCode errorCode;

			::cyw43_arch_enable_sta_mode();

			if ((errorCode = ::cyw43_arch_wifi_connect_blocking(ssid.GetCString(), nullptr, CYW43_AUTH_OPEN)) != PICO_ERROR_NONE)
			{

				throw OS::SystemException(
					"cyw43_arch_wifi_connect_timeout_ms",
					errorCode
				);
			}

			isStation     = True;
			isAccessPoint = False;
			isConnected   = True;
			isListening   = False;
		}
		// @throw AL::Exception
		static Void Connect(const String& ssid, const String& password, AuthTypes authType)
		{
			AL_ASSERT(
				IsOpen(),
				"CYW43 not open"
			);

			uint32        _authType;
			OS::ErrorCode errorCode;

			if (!AuthTypes_ToCYW43(_authType, authType))
			{

				throw Exception(
					"AuthType not supported"
				);
			}

			::cyw43_arch_enable_sta_mode();

			if ((errorCode = ::cyw43_arch_wifi_connect_blocking(ssid.GetCString(), password.GetCString(), _authType)) != PICO_ERROR_NONE)
			{

				throw OS::SystemException(
					"cyw43_arch_wifi_connect_blocking",
					errorCode
				);
			}

			isStation     = True;
			isAccessPoint = False;
			isConnected   = True;
			isListening   = False;
		}

		// @throw AL::Exception
		// @return AL::False on timeout
		static Bool TryConnect(const String& ssid, TimeSpan timeout = TimeSpan::Infinite)
		{
			AL_ASSERT(
				IsOpen(),
				"CYW43 not open"
			);

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

			isStation     = True;
			isAccessPoint = False;
			isConnected   = True;
			isListening   = False;

			return True;
		}
		// @throw AL::Exception
		// @return AL::False on timeout
		static Bool TryConnect(const String& ssid, const String& password, AuthTypes authType, TimeSpan timeout = TimeSpan::Infinite)
		{
			AL_ASSERT(
				IsOpen(),
				"CYW43 not open"
			);

			uint32        _authType;
			OS::ErrorCode errorCode;

			if (!AuthTypes_ToCYW43(_authType, authType))
			{

				throw Exception(
					"AuthType not supported"
				);
			}

			::cyw43_arch_enable_sta_mode();

			if ((errorCode = ::cyw43_arch_wifi_connect_timeout_ms(ssid.GetCString(), password.GetCString(), _authType, static_cast<::uint32_t>(timeout.ToMilliseconds()))) != PICO_ERROR_NONE)
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

			isStation     = True;
			isAccessPoint = False;
			isConnected   = True;
			isListening   = False;

			return True;
		}

		// @throw AL::Exception
		static Void Listen(const String& ssid)
		{
			AL_ASSERT(
				IsOpen(),
				"CYW43 not open"
			);

			::cyw43_arch_enable_ap_mode(
				ssid.GetCString(),
				nullptr,
				CYW43_AUTH_OPEN
			);

			isStation     = False;
			isAccessPoint = True;
			isConnected   = False;
			isListening   = True;
		}
		// @throw AL::Exception
		static Void Listen(const String& ssid, const String& password, AuthTypes authType)
		{
			AL_ASSERT(
				IsOpen(),
				"CYW43 not open"
			);

			uint32 _authType;

			if (!AuthTypes_ToCYW43(_authType, authType))
			{

				throw Exception(
					"AuthType not supported"
				);
			}

			::cyw43_arch_enable_ap_mode(
				ssid.GetCString(),
				(_authType == CYW43_AUTH_OPEN) ? nullptr : password.GetCString(),
				_authType
			);

			isStation     = False;
			isAccessPoint = True;
			isConnected   = False;
			isListening   = True;
		}

		// @throw AL::Exception
		static Void SetPowerManagement(PowerLevels value)
		{
			AL_ASSERT(
				IsOpen(),
				"CYW43 not open"
			);

			OS::ErrorCode errorCode = ::PICO_ERROR_INVALID_ARG;

			switch (value)
			{
				case PowerLevels::Default:
					errorCode = ::cyw43_wifi_pm(&cyw43_state, CYW43_DEFAULT_PM);
					break;

				case PowerLevels::Aggressive:
					errorCode = ::cyw43_wifi_pm(&cyw43_state, CYW43_AGGRESSIVE_PM);
					break;

				case PowerLevels::Performance:
					errorCode = ::cyw43_wifi_pm(&cyw43_state, CYW43_PERFORMANCE_PM);
					break;

				default:
					throw NotImplementedException();
			}

			if (errorCode != 0)
			{

				throw OS::SystemException(
					"cyw43_wifi_pm",
					errorCode
				);
			}
		}

	private:
		static int OnScan(void* lpParam, const cyw43_ev_scan_result_t* lpResult)
		{
			if (auto lpContext = reinterpret_cast<ScanContext*>(lpParam); !lpContext->Stop)
			{
				Network network =
				{
					.RSSI     = lpResult->rssi,
					.SSID     = String(reinterpret_cast<const char*>(lpResult->ssid), lpResult->ssid_len),
					.BSSID    = BSSID_ToString(lpResult->bssid),
					.Channel  = lpResult->channel,
					.AuthType = AuthTypes::Open
				};

				if (AuthTypes_FromCYW43(network.AuthType, lpResult->auth_mode))
				{
					if (!(*lpContext->lpCallback)(network))
					{

						lpContext->Stop = True;
					}
				}
			}

			return 0;
		}

	private:
		template<typename F, typename ... TArgs>
		static Void LWIP_Sync(F&& function, TArgs ... args)
		{
			::cyw43_arch_lwip_begin();

			function(
				Forward<TArgs>(args) ...
			);

			::cyw43_arch_lwip_end();
		}

	private:
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

		static Bool AuthTypes_ToCYW43(uint32& result, AuthTypes value)
		{
			switch (value)
			{
				case AuthTypes::Open:
					result = CYW43_AUTH_OPEN;
					return True;

				case AuthTypes::WEP_PSK:
					// TODO: implement this if/when the pico sdk supports it
					return False;

				case AuthTypes::WPA_TKIP_PSK:
					result = CYW43_AUTH_WPA_TKIP_PSK;
					return True;

				case AuthTypes::WPA2_AES_PSK:
					result = CYW43_AUTH_WPA2_AES_PSK;
					return True;

				case AuthTypes::WPA2_MIXED_PSK:
					result = CYW43_AUTH_WPA2_MIXED_PSK;
					return True;
			}

			return False;
		}

		static Bool AuthTypes_FromCYW43(AuthTypes& result, uint8 value)
		{
			// based on cyw43-driver/src/cyw43_ll.c:cyw43_ll_wifi_parse_scan_result

			if (value == 0x00)
			{
				result = AuthTypes::Open;

				return True;
			}

			if (value & 0x01)
			{
				result = AuthTypes::WEP_PSK;

				return True;
			}

			if (value & 0x02)
			{
				result = AuthTypes::WPA_TKIP_PSK;

				return True;
			}

			if (value & 0x04)
			{
				result = AuthTypes::WPA2_MIXED_PSK;

				return True;
			}

			return False;
		}
	};
}
