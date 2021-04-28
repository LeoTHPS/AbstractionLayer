#pragma once
#include "AL/Common.hpp"

#include "NetworkAuthAlgorithms.hpp"
#include "NetworkCipherAlgorithms.hpp"

#include "AL/Collections/Set.hpp"

namespace AL::WiFi
{
	struct NetworkInformation
	{
		int32                    RSSI;
		String                   SSID;
		String                   BSSID;
		uint32                   Frequency;
		uint32                   LinkQuality;
		uint32                   SignalQuality;
		Collections::Set<uint16> TransferRates;
		NetworkAuthAlgorithms    AuthAlgorithm;
		NetworkCipherAlgorithms  CipherAlgorithm;
	};
}
