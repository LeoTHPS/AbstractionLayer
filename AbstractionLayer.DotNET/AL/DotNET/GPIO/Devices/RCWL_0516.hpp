#pragma once
#include "AL/DotNET/Common.hpp"

#include <AL/GPIO/Devices/RCWL_0516.hpp>

namespace AL::DotNET::GPIO::Devices
{
	public ref class RCWL_0516
	{
		AL::GPIO::Devices::RCWL_0516* const lpDevice;

	public:
		RCWL_0516(AL::GPIO::DeviceId deviceId, AL::GPIO::PinNumber pinNumber)
			: lpDevice(
				new AL::GPIO::Devices::RCWL_0516(
					deviceId,
					pinNumber
				)
			)
		{
		}

		virtual ~RCWL_0516()
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
		void Read([System::Runtime::InteropServices::Out] System::Boolean% value)
		{
			bool _value;

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
