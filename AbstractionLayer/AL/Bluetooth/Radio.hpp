#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
	#include <bluetoothapis.h>

	#pragma comment(lib, "Bthprops.lib")
#endif

namespace AL::Bluetooth
{
	class Radio;

	// @throw AL::Exceptions::Exception
	typedef Function<void(Radio& radio)> RadioEnumCallback;

	class Radio
	{
		bool isOpen = false;

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
		HANDLE hRadio;
		BLUETOOTH_RADIO_INFO info;
#endif

		String name;
		uint64 address;
		uint64 deviceClass;
		uint16 subversion;
		uint16 manufacturer;

		Radio(const Radio&) = delete;

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
		Radio(HANDLE hRadio, BLUETOOTH_RADIO_INFO&& info)
			: isOpen(
				true
			),
			hRadio(
				hRadio
			),
			info(
				Move(info)
			),
			name(
				String::Format(
					"%S",
					this->info.szName
				)
			),
			address(
				this->info.address.ullLong
			),
			deviceClass(
				this->info.ulClassofDevice
			),
			subversion(
				this->info.lmpSubversion
			),
			manufacturer(
				this->info.manufacturer
			)
		{
		}
#endif

	public:
		// @throw AL::Exceptions::Exception
		static void Enumerate(const RadioEnumCallback& callback)
		{
#if defined(AL_PLATFORM_LINUX)
			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			Radio radio;

			HANDLE hRadio;
			
			BLUETOOTH_RADIO_INFO info = { 0 };
			info.dwSize = sizeof(BLUETOOTH_RADIO_INFO);

			BLUETOOTH_FIND_RADIO_PARAMS params = { 0 };
			params.dwSize = sizeof(BLUETOOTH_FIND_RADIO_PARAMS);

			if (auto hFind = BluetoothFindFirstRadio(&params, &hRadio))
			{
				do
				{
					switch (auto errorCode = BluetoothGetRadioInfo(hRadio, &info))
					{
						case ERROR_SUCCESS:
							break;

						case ERROR_INVALID_PARAMETER:
						case ERROR_REVISION_MISMATCH:
							CloseHandle(hRadio);
							BluetoothFindRadioClose(hFind);
							throw Exceptions::SystemException(
								"BluetoothGetRadioInfo",
								errorCode
							);
					}

					radio = Radio(
						hRadio,
						Move(info)
					);

					try
					{
						callback(
							radio
						);
					}
					catch (Exceptions::Exception&)
					{
						BluetoothFindRadioClose(
							hFind
						);

						throw;
					}
				} while (BluetoothFindNextRadio(hFind, &hRadio));

				auto lastErrorCode = OS::GetLastError();

				BluetoothFindRadioClose(
					hFind
				);

				if (lastErrorCode != ERROR_NO_MORE_ITEMS)
				{

					throw Exceptions::SystemException(
						"BluetoothFindNextRadio",
						lastErrorCode
					);
				}
			}
			else
			{

				throw Exceptions::SystemException(
					"BluetoothFindFirstRadio"
				);
			}
#else
			throw Exceptions::NotImplementedException();
#endif
		}

		Radio()
		{
		}

		Radio(Radio&& radio)
			: isOpen(
				radio.isOpen
			),
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			hRadio(
				radio.hRadio
			),
			info(
				Move(radio.info)
			),
#endif
			name(
				Move(radio.name)
			),
			address(
				radio.address
			),
			deviceClass(
				radio.deviceClass
			),
			subversion(
				radio.subversion
			),
			manufacturer(
				radio.manufacturer
			)
		{
			radio.isOpen = false;
		}

		virtual ~Radio()
		{
			Close();
		}

		bool IsOpen() const
		{
			return isOpen;
		}

		auto& GetName() const
		{
			return name;
		}

		auto GetHandle() const
		{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			return hRadio;
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

		auto GetSubVersion() const
		{
			return subversion;
		}

		auto GetManufacturer() const
		{
			return manufacturer;
		}

		void Close()
		{
			if (IsOpen())
			{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
				CloseHandle(
					hRadio
				);
#endif

				isOpen = false;
			}
		}

		Radio& operator = (Radio&& radio)
		{
			Close();

			isOpen = radio.isOpen;
			radio.isOpen = false;

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			hRadio = radio.hRadio;

			info = Move(
				radio.info
			);
#endif

			name = Move(
				radio.name
			);

			address = radio.address;

			deviceClass = radio.deviceClass;

			subversion = radio.subversion;

			manufacturer = radio.manufacturer;

			return *this;
		}
	};
}
