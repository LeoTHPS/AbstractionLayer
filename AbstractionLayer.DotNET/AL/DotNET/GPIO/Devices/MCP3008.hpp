#pragma once
#include "AL/DotNET/Common.hpp"

#include <AL/GPIO/Devices/MCP3008.hpp>

namespace AL::DotNET::GPIO::Devices
{
	public ref class MCP3008
	{
		AL::GPIO::Devices::MCP3008* const lpDevice;

	public:
		MCP3008(AL::GPIO::SPIBusId spiBusId, AL::GPIO::SPIDeviceId spiDeviceId)
			: lpDevice(
				new AL::GPIO::Devices::MCP3008(
					spiBusId,
					spiDeviceId
				)
			)
		{
		}

		MCP3008(AL::GPIO::SPIBusId spiBusId, AL::GPIO::SPIDeviceId spiDeviceId, AL::GPIO::PinNumber csPinNumber)
			: lpDevice(
				new AL::GPIO::Devices::MCP3008(
					spiBusId,
					spiDeviceId,
					csPinNumber
				)
			)
		{
		}

		virtual ~MCP3008()
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
		void Read(System::Byte channel, [System::Runtime::InteropServices::Out] System::UInt16% value)
		{
			uint16 _value;

			try
			{
				lpDevice->Read(
					channel,
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
