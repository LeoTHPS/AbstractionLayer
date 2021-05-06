#pragma once
#include "AL/Common.hpp"

#include "NetworkInformation.hpp"

#include "AL/Collections/Map.hpp"
#include "AL/Collections/List.hpp"

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
	#include <wlanapi.h>

	#pragma comment(lib, "Ole32.lib")
	#pragma comment(lib, "wlanapi.lib")
#endif

namespace AL::WiFi
{
	class Device;

	// @throw AL::Exceptions::Exception
	typedef Function<void(Device& device)>                        DeviceEnumCallback;
	// @throw AL::Exceptions::Exception
	typedef Function<void(const NetworkInformation& information)> DeviceScanCallback;

	class Device
	{
		bool isOpen = false;
		bool isConnected = false;

#if defined(AL_PLATFORM_LINUX)
		typedef void* DeviceGUID;

#elif defined(AL_PLATFORM_WINDOWS)
		typedef ::GUID DeviceGUID;

		HANDLE hClient;
#endif
		
		DeviceGUID guid;
		String     name;

		Device(Device&&) = delete;
		Device(const Device&) = delete;

	public:
		// @throw AL::Exceptions::Exception
		static void Open(Device& device, const DeviceGUID& guid)
		{
			#if defined(AL_PLATFORM_LINUX)
			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			HANDLE hClient;
			DWORD currentVersion;
			OS::ErrorCode errorCode;

			if ((errorCode = WlanOpenHandle(2, nullptr, &currentVersion, &hClient)) != ERROR_SUCCESS)
			{

				throw Exceptions::SystemException(
					"WlanOpenHandle",
					errorCode
				);
			}

			device.Close();

			device.isOpen = true;
			device.guid = guid;
			device.hClient = hClient;
#endif
		}

		// @throw AL::Exceptions::Exception
		static void Enumerate(const DeviceEnumCallback& callback)
		{
#if defined(AL_PLATFORM_LINUX)
			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			HANDLE hClient;
			DWORD currentVersion;
			OS::ErrorCode errorCode;

			if ((errorCode = WlanOpenHandle(2, nullptr, &currentVersion, &hClient)) != ERROR_SUCCESS)
			{

				throw Exceptions::SystemException(
					"WlanOpenHandle",
					errorCode
				);
			}

			PWLAN_INTERFACE_INFO_LIST interfaceList;

			if ((errorCode = WlanEnumInterfaces(hClient, nullptr, &interfaceList)) != ERROR_SUCCESS)
			{
				WlanCloseHandle(
					hClient,
					nullptr
				);

				throw Exceptions::SystemException(
					"WlanEnumInterfaces",
					errorCode
				);
			}

			Device device;
			device.isOpen = true;
			device.hClient = hClient;

			for (size_t i = 0; i < interfaceList->dwNumberOfItems; ++i)
			{
				auto lpInterface = &interfaceList->InterfaceInfo[i];

				device.guid = lpInterface->InterfaceGuid;

				device.name = String::Format(
					"%S",
					lpInterface->strInterfaceDescription
				);

				try
				{
					callback(
						device
					);
				}
				catch (Exceptions::Exception&)
				{
					WlanFreeMemory(
						interfaceList
					);

					WlanCloseHandle(
						hClient,
						nullptr
					);

					throw;
				}
			}

			WlanFreeMemory(
				interfaceList
			);

			WlanCloseHandle(
				hClient,
				nullptr
			);
#endif
		}

		Device()
		{
		}

		virtual ~Device()
		{
			Close();
		}

		bool IsOpen() const
		{
			return hClient != NULL;
		}

		bool IsConnected() const
		{
			return isConnected;
		}

		auto& GetGUID() const
		{
			return guid;
		}

		auto& GetName() const
		{
			return name;
		}

		// @throw AL::Exceptions::Exception
		// @return false if duration elapsed
		bool Scan(const DeviceScanCallback& callback, TimeSpan maxWaitTime = TimeSpan::Infinite)
		{
			AL_ASSERT(IsOpen(), "Device not open");

#if defined(AL_PLATFORM_LINUX)
			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			struct WLAN_NOTIFICATION_CALLBACK_CONTEXT
			{
				Device* const      lpDevice;
				OS::MutexCondition Condition;

				bool               Success;

				WLAN_NOTIFICATION_CALLBACK_CONTEXT(Device& device)
					: lpDevice(
						&device
					),
					Success(
						false
					)
				{
				}
			};

			WLAN_NOTIFICATION_CALLBACK notificationCallback = [](PWLAN_NOTIFICATION_DATA _lpData, PVOID _lpContext)
			{
				auto lpContext = reinterpret_cast<WLAN_NOTIFICATION_CALLBACK_CONTEXT*>(
					_lpContext
				);

				switch (_lpData->NotificationCode)
				{
					case wlan_notification_acm_scan_fail:
						lpContext->Success = false;
						lpContext->Condition.WakeOne();
						break;

					case wlan_notification_acm_scan_complete:
						lpContext->Success = true;
						lpContext->Condition.WakeOne();
						break;
				}
			};

			WLAN_NOTIFICATION_CALLBACK_CONTEXT notificationCallbackContext(
				*this
			);

			DWORD errorCode;
			DWORD dwPrevNotifSource = 0;

			if ((errorCode = WlanRegisterNotification(hClient, WLAN_NOTIFICATION_SOURCE_ACM, TRUE, notificationCallback, &notificationCallbackContext, nullptr, &dwPrevNotifSource)) != ERROR_SUCCESS)
			{

				throw Exceptions::SystemException(
					"WlanRegisterNotification",
					errorCode
				);
			}

			if ((errorCode = WlanScan(hClient, &GetGUID(), nullptr, nullptr, nullptr)) != ERROR_SUCCESS)
			{
				WlanRegisterNotification(
					hClient,
					WLAN_NOTIFICATION_SOURCE_NONE,
					TRUE,
					NULL,
					nullptr,
					nullptr,
					&dwPrevNotifSource
				);

				throw Exceptions::SystemException(
					"WlanScan",
					errorCode
				);
			}

			if (!notificationCallbackContext.Condition.Sleep(maxWaitTime))
			{
				WlanRegisterNotification(
					hClient,
					WLAN_NOTIFICATION_SOURCE_NONE,
					TRUE,
					NULL,
					nullptr,
					nullptr,
					&dwPrevNotifSource
				);

				return false;
			}

			WlanRegisterNotification(
				hClient,
				WLAN_NOTIFICATION_SOURCE_NONE,
				TRUE,
				NULL,
				nullptr,
				nullptr,
				&dwPrevNotifSource
			);

			PWLAN_BSS_LIST lpAvailableNetworksBSS;

			if ((errorCode = WlanGetNetworkBssList(hClient, &GetGUID(), nullptr, dot11_BSS_type_any, FALSE, nullptr, &lpAvailableNetworksBSS)) != ERROR_SUCCESS)
			{

				throw Exceptions::SystemException(
					"WlanGetNetworkBssList",
					errorCode
				);
			}

			PWLAN_AVAILABLE_NETWORK_LIST lpAvailableNetworks;

			if ((errorCode = WlanGetAvailableNetworkList(hClient, &GetGUID(), 0, nullptr, &lpAvailableNetworks)) != ERROR_SUCCESS)
			{
				WlanFreeMemory(
					lpAvailableNetworksBSS
				);

				throw Exceptions::SystemException(
					"WlanGetAvailableNetworkList",
					errorCode
				);
			}

			auto DOT11_SSID_ToString = [](const DOT11_SSID& _ssid)
			{
				return String(
					reinterpret_cast<const String::Char*>(_ssid.ucSSID),
					_ssid.uSSIDLength
				);
			};

			auto DOT11_MAC_ADDRESS_ToString = [](const DOT11_MAC_ADDRESS& _address)
			{
				String string(
					(sizeof(DOT11_MAC_ADDRESS) * 2) + (sizeof(DOT11_MAC_ADDRESS) - 1)
				);

				for (size_t i = 0; i < sizeof(DOT11_MAC_ADDRESS); ++i)
				{
					auto chunk = HexConverter::Encode(
						&_address[i],
						1
					);

					if (i != 0)
					{
						
						string.Append(
							':'
						);
					}

					string.Append(
						chunk
					);
				}

				return string;
			};

			//               DOT11_SSID
			Collections::Map<String, NetworkInformation> availableNetworks;

			for (DWORD i = 0; i < lpAvailableNetworksBSS->dwNumberOfItems; ++i)
			{
				auto dot11SSID = DOT11_SSID_ToString(
					lpAvailableNetworksBSS->wlanBssEntries[i].dot11Ssid
				);

				if (dot11SSID.GetLength() == 0)
				{

					continue;
				}

				auto lpNetwork = &lpAvailableNetworksBSS->wlanBssEntries[i];

				auto lpNetworkInformation = &availableNetworks[dot11SSID];

				lpNetworkInformation->RSSI = lpNetwork->lRssi;

				lpNetworkInformation->SSID = Move(
					dot11SSID
				);

				lpNetworkInformation->BSSID = DOT11_MAC_ADDRESS_ToString(
					lpNetwork->dot11Bssid
				);

				lpNetworkInformation->Frequency = lpNetwork->ulChCenterFrequency;

				lpNetworkInformation->LinkQuality = lpNetwork->uLinkQuality;

				for (size_t i = 0; i < lpNetwork->wlanRateSet.uRateSetLength; ++i)
				{
					lpNetworkInformation->TransferRates.Add(
						lpNetwork->wlanRateSet.usRateSet[i]
					);
				}
			}

			for (DWORD i = lpAvailableNetworks->dwIndex; i < lpAvailableNetworks->dwNumberOfItems; ++i)
			{
				auto dot11SSID = DOT11_SSID_ToString(
					lpAvailableNetworks->Network[i].dot11Ssid
				);

				auto it = availableNetworks.Find(
					dot11SSID
				);

				if (it == availableNetworks.end())
				{

					continue;
				}

				auto lpNetwork = &lpAvailableNetworks->Network[i];

				auto lpNetworkInformation = &it->second;

				lpNetworkInformation->AuthAlgorithm = NetworkAuthAlgorithms::None;
				lpNetworkInformation->CipherAlgorithm = NetworkCipherAlgorithms::None;

				if (lpNetwork->bSecurityEnabled)
				{
					switch (lpNetwork->dot11DefaultAuthAlgorithm)
					{
						case DOT11_AUTH_ALGO_80211_OPEN:
							lpNetworkInformation->AuthAlgorithm = NetworkAuthAlgorithms::Open;
							break;

						case DOT11_AUTH_ALGO_80211_SHARED_KEY:
							lpNetworkInformation->AuthAlgorithm = NetworkAuthAlgorithms::SharedKey;
							break;

						case DOT11_AUTH_ALGO_WPA:
							lpNetworkInformation->AuthAlgorithm = NetworkAuthAlgorithms::WPA;
							break;

						case DOT11_AUTH_ALGO_WPA_PSK:
							lpNetworkInformation->AuthAlgorithm = NetworkAuthAlgorithms::WPA_PSK;
							break;

						case DOT11_AUTH_ALGO_WPA_NONE:
							lpNetworkInformation->AuthAlgorithm = NetworkAuthAlgorithms::WPA_None;
							break;

						case DOT11_AUTH_ALGO_RSNA:
							lpNetworkInformation->AuthAlgorithm = NetworkAuthAlgorithms::RSNA;
							break;

						case DOT11_AUTH_ALGO_RSNA_PSK:
							lpNetworkInformation->AuthAlgorithm = NetworkAuthAlgorithms::RSNA_PSK;
							break;

						default:
							lpNetworkInformation->AuthAlgorithm = NetworkAuthAlgorithms::Undefined;
							break;
					}

					switch (lpNetwork->dot11DefaultCipherAlgorithm)
					{
						case DOT11_CIPHER_ALGO_NONE:
							lpNetworkInformation->CipherAlgorithm = NetworkCipherAlgorithms::None;
							break;

						case DOT11_CIPHER_ALGO_WEP:
							lpNetworkInformation->CipherAlgorithm = NetworkCipherAlgorithms::WEP;
							break;

						case DOT11_CIPHER_ALGO_WEP40:
							lpNetworkInformation->CipherAlgorithm = NetworkCipherAlgorithms::WEP40;
							break;

						case DOT11_CIPHER_ALGO_WEP104:
							lpNetworkInformation->CipherAlgorithm = NetworkCipherAlgorithms::WEP104;
							break;

						case DOT11_CIPHER_ALGO_TKIP:
							lpNetworkInformation->CipherAlgorithm = NetworkCipherAlgorithms::TKIP;
							break;

						case DOT11_CIPHER_ALGO_CCMP:
							lpNetworkInformation->CipherAlgorithm = NetworkCipherAlgorithms::CCMP;
							break;

						default:
							lpNetworkInformation->CipherAlgorithm = NetworkCipherAlgorithms::Undefined;
							break;
					}
				}

				lpNetworkInformation->SignalQuality = lpNetwork->wlanSignalQuality;

				try
				{
					callback(
						*lpNetworkInformation
					);
				}
				catch (Exceptions::Exception&)
				{
					WlanFreeMemory(
						lpAvailableNetworks
					);

					WlanFreeMemory(
						lpAvailableNetworksBSS
					);

					throw;
				}
			}

			WlanFreeMemory(
				lpAvailableNetworks
			);

			WlanFreeMemory(
				lpAvailableNetworksBSS
			);
#endif

			return true;
		}

		// @throw AL::Exceptions::Exception
		void Connect(const NetworkInformation& network)
		{
			AL_ASSERT(IsOpen(), "Device not open");

#if defined(AL_PLATFORM_LINUX)
			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			OS::ErrorCode errorCode;
			
			auto wSSID = WString::Format(
				L"%S",
				network.SSID.GetCString()
			);

			WLAN_CONNECTION_PARAMETERS params;
			params.dwFlags = 0;
			params.strProfile = wSSID.GetCString();
			params.dot11BssType = dot11_BSS_type_any;
			params.pDot11Ssid = nullptr;
			params.pDesiredBssidList = nullptr;
			params.wlanConnectionMode = wlan_connection_mode_profile;

			if ((errorCode = WlanConnect(hClient, &GetGUID(), &params, nullptr)) != ERROR_SUCCESS)
			{
				// TODO: detect missing profile

				throw Exceptions::SystemException(
					"WlanConnect",
					errorCode
				);
			}
#endif

			isConnected = true;
		}

		void Disconnect()
		{
			if (IsConnected())
			{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
				WlanDisconnect(
					hClient,
					&GetGUID(),
					nullptr
				);
#endif

				isConnected = false;
			}
		}

	private:
		void Close()
		{
			if (IsOpen())
			{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
				WlanCloseHandle(
					hClient,
					nullptr
				);
#endif

				isOpen = false;
			}
		}
	};
}
