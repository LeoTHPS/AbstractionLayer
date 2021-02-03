#pragma once
#include "AL/DotNET/Common.hpp"

#include <AL/GPIO/Devices/DHT22.hpp>

namespace AL::DotNET::GPIO::Devices
{
	public ref class DHT22
	{
		AL::GPIO::Devices::DHT22* const lpDevice;

	public:
		DHT22(AL::GPIO::DeviceId deviceId, AL::GPIO::PinNumber pinNumber)
			: lpDevice(
				new AL::GPIO::Devices::DHT22(
					deviceId,
					pinNumber
				)
			)
		{
		}

		virtual ~DHT22()
		{
			delete lpDevice;
		}

		bool IsOpen()
		{
			return lpDevice->IsOpen();
		}

		/// <exception cref="AL::Exceptions::Exception" />
		void Open()
		{
			try
			{
				lpDevice->Open();
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}
		}

		void Close()
		{
			lpDevice->Close();
		}

		/// <exception cref="AL::Exceptions::Exception" />
		void Read([System::Runtime::InteropServices::Out] System::UInt16% value)
		{
			uint16 _value;

			try
			{
				lpDevice->Read(
					_value
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}

			value = _value;
		}
	};
}
