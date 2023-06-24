#pragma once
#include "AL/Common.hpp"

#include "AL/OS/ErrorCode.hpp"
#include "AL/OS/SystemException.hpp"

#include "AL/Collections/LinkedList.hpp"
#include "AL/Collections/Dictionary.hpp"

#if defined(AL_PLATFORM_PICO)
	#include "PicoW/CYW43.hpp"
#elif defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
	#include <wlanapi.h>

	#if defined(AL_COMPILER_MSVC)
		#pragma comment(lib, "Ole32.lib")
		#pragma comment(lib, "wlanapi.lib")
	#endif
#else
	#error Platform not supported
#endif

namespace AL::Hardware
{
	enum class WiFiNetworkAuthAlgorithms : uint8
	{
		None,
		Open,
		SharedKey,
		WPA,
		WPA_PSK,
		WPA_None,
		RSNA,
		RSNA_PSK,

		Undefined
	};

	enum class WiFiNetworkCipherAlgorithms : uint8
	{
		None,
		WEP,
		WEP40,
		WEP104,
		TKIP,
		CCMP,

		Undefined
	};

	struct WiFiRadioInformation
	{
#if defined(AL_PLATFORM_PICO)

#elif defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)

#endif
	};

	struct WiFiNetworkInformation
	{
		int32                           RSSI;
		String                          SSID;
		String                          BSSID;
		uint32                          Frequency;
		uint32                          LinkQuality;
		uint32                          SignalQuality;
		Collections::LinkedList<uint16> TransferRates;
		WiFiNetworkAuthAlgorithms       AuthAlgorithm;
		WiFiNetworkCipherAlgorithms     CipherAlgorithm;
	};

	// @throw AL::Exception
	typedef Function<Void()> WiFiRadioEnumCallback;

	// @throw AL::Exception
	typedef Function<Void(const WiFiNetworkInformation& networkInformation)> WiFiRadioEnumNetworksCallback;

	class WiFiRadio
	{
		Bool isOpen      = False;
		Bool isConnected = False;

		WiFiRadioInformation   information;
		WiFiNetworkInformation networkInfomation;

	public:
		// @throw AL::Exception
		static Void Enumerate(const WiFiRadioEnumCallback& callback);

		// @throw AL::Exception
		// @return AL::False if already open
		static Bool Open(WiFiRadio& radio, const WiFiRadioInformation& information);

		WiFiRadio();

		WiFiRadio(WiFiRadio&& radio);

		virtual ~WiFiRadio();

		Bool IsOpen() const
		{
			return isOpen;
		}

		Bool IsConnected() const
		{
			return isConnected;
		}

		// @throw AL::Exception
		// @return AL::False on timeout
		Bool Connect(const String& ssid, const String& password, WiFiNetworkAuthAlgorithms authAlgorithm, TimeSpan timeout);

		// @throw AL::Exception
		// @return AL::False on timeout
		Bool EnumerateNetworks(const WiFiRadioEnumNetworksCallback& callback, TimeSpan timeout);

		Void Close();

		Void Disconnect();

		WiFiRadio& operator = (WiFiRadio&& radio);

		Bool operator == (const WiFiRadio& radio) const;
		Bool operator != (const WiFiRadio& radio) const
		{
			if (operator==(radio))
			{

				return False;
			}

			return True;
		}
	};
}
