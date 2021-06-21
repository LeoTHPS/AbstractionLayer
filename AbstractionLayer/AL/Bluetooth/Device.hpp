#pragma once
#include "AL/Common.hpp"

#include "Radio.hpp"

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
	#include <ws2bth.h>
	#include <bthsdpdef.h>
	#include <bluetoothapis.h>
	#include <WinSock2.h>

	#pragma comment(lib, "Ws2_32.lib")
	#pragma comment(lib, "Bthprops.lib")
#endif

namespace AL::Bluetooth
{
	class Device;
	
	// @throw AL::Exceptions::Exception
	typedef Function<void(Device& device)> DeviceEnumCallback;

	class Device
	{
		bool isOpen = false;
		bool isAuthenticated = false;

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
		BLUETOOTH_DEVICE_INFO info;
#endif

		String name;
		uint64 address;
		uint64 deviceClass;

		DateTime lastSeen;
		DateTime lastUsed;

		Radio* lpRadio = nullptr;

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
		Device(Radio& radio, BLUETOOTH_DEVICE_INFO&& info)
			: isOpen(
				true
			),
			info(
				Move(info)
			),
			name(
				String::Format(
					"%S",
					info.szName
				)
			),
			address(
				info.Address.ullLong
			),
			deviceClass(
				info.ulClassofDevice
			),
			lastSeen(
				SystemTimeToDateTime(
					this->info.stLastSeen
				)
			),
			lastUsed(
				SystemTimeToDateTime(
					this->info.stLastUsed
				)
			),
			lpRadio(
				&radio
			)
		{
		}
#endif

		Device(const Device&) = delete;

	public:
		// @throw AL::Exceptions::Exception
		static void Enumerate(Radio& radio, const DeviceEnumCallback& callback)
		{
#if defined(AL_PLATFORM_LINUX)
			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			Device device;

			BLUETOOTH_DEVICE_INFO info = { 0 };
			info.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
			
			BLUETOOTH_DEVICE_SEARCH_PARAMS params = { 0 };
			params.dwSize = sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS);
			params.hRadio = radio.GetHandle();
			params.fIssueInquiry = TRUE;
			params.fReturnUnknown = TRUE;
			params.fReturnConnected = TRUE;
			params.fReturnRemembered = TRUE;
			params.fReturnAuthenticated = TRUE;
			params.cTimeoutMultiplier = 8;

			if (auto hFind = BluetoothFindFirstDevice(&params, &info))
			{
				do
				{
					device = Device(
						radio,
						Move(info)
					);

					try
					{
						callback(
							device
						);
					}
					catch (Exceptions::Exception&)
					{
						BluetoothFindDeviceClose(
							hFind
						);

						throw;
					}
				} while (BluetoothFindNextDevice(hFind, &info));

				auto lastErrorCode = OS::GetLastError();

				BluetoothFindDeviceClose(
					hFind
				);

				if (lastErrorCode != ERROR_NO_MORE_ITEMS)
				{

					throw Exceptions::SystemException(
						"BluetoothFindNextDevice",
						lastErrorCode
					);
				}
			}
			else
			{

				throw Exceptions::SystemException(
					"BluetoothFindFirstDevice"
				);
			}
#else
			throw Exceptions::NotImplementedException();
#endif
		}

		Device()
		{
		}

		Device(Device&& device)
			: isOpen(
				device.isOpen
			),
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			info(
				Move(device.info)
			),
#endif
			name(
				Move(device.name)
			),
			address(
				device.address
			),
			deviceClass(
				device.deviceClass
			),
			lastSeen(
				Move(device.lastSeen)
			),
			lastUsed(
				Move(device.lastUsed)
			),
			lpRadio(
				device.lpRadio
			)
		{
			device.isOpen = false;

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)

#endif

			device.lpRadio = nullptr;
		}

		virtual ~Device()
		{
			Close();
		}

		bool IsOpen() const
		{
			return isOpen;
		}

		bool IsConnected() const
		{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			return info.fConnected != 0;
#endif

			return false;
		}

		bool IsRemembered() const
		{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			return info.fRemembered != 0;
#endif

			return false;
		}

		bool IsAuthenticated() const
		{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			return info.fAuthenticated != 0;
#endif

			return false;
		}

		auto& GetLastSeenTime() const
		{
			return lastSeen;
		}

		auto& GetLastUsedTime() const
		{
			return lastUsed;
		}

		auto& GetName() const
		{
			return name;
		}

		auto& GetRadio()
		{
			return *lpRadio;
		}
		auto& GetRadio() const
		{
			return *lpRadio;
		}

		auto GetHandle() const
		{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			
#endif
		}

		auto GetAddress() const
		{
			return address;
		}

		auto GetDeviceClass() const
		{
			return deviceClass;
		}

		void Close()
		{
			if (IsOpen())
			{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)

#endif

				isOpen = false;
			}
		}

		Device& operator = (Device&& device)
		{
			Close();

			isOpen = device.isOpen;
			device.isOpen = false;

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			info = Move(
				device.info
			);
#endif

			name = Move(
				device.name
			);

			address = device.address;

			deviceClass = device.deviceClass;

			lastSeen = Move(
				device.lastSeen
			);

			lastUsed = Move(
				device.lastUsed
			);

			lpRadio = device.lpRadio;
			device.lpRadio = nullptr;

			return *this;
		}

	private:
#if defined(AL_PLATFORM_WINDOWS)
		static DateTime SystemTimeToDateTime(const SYSTEMTIME& time)
		{
			DateTime dateTime;

			dateTime.Year = static_cast<uint32>(
				time.wYear
			);
			dateTime.Month = static_cast<DateTime::Months>(
				time.wMonth
			);
			dateTime.Day = static_cast<uint32>(
				time.wDay
			);
			dateTime.DayOfWeek = static_cast<DateTime::DaysOfWeek>(
				time.wDayOfWeek
			);
			dateTime.Hour = static_cast<uint32>(
				time.wHour
			);
			dateTime.Minutes = static_cast<uint32>(
				time.wMinute
			);
			dateTime.Seconds = static_cast<uint32>(
				time.wSecond
			);
			dateTime.Milliseconds = static_cast<uint32>(
				time.wMilliseconds
			);

			return dateTime;
		}
#endif
	};
}
