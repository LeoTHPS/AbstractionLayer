#pragma once
#include "AL/DotNET/Common.hpp"

#include <AL/GPIO/Devices/BN_180.hpp>

namespace AL::DotNET::GPIO::Devices
{
	public ref class BN_180
	{
		AL::GPIO::Devices::BN_180* const lpDevice;

	public:
		explicit BN_180(System::String^ deviceName)
			: lpDevice(
				new AL::GPIO::Devices::BN_180(
					Marshal::ToNativeString(
						deviceName
					)
				)
			)
		{
		}

		virtual ~BN_180()
		{
			delete lpDevice;
		}

		bool IsOpen()
		{
			return lpDevice->IsOpen();
		}

		/// <exception cref="AL::DotNET::Exceptions::Exception" />
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

		/// <exception cref="AL::DotNET::Exceptions::Exception" />
		void Read([System::Runtime::InteropServices::Out] System::String^% value)
		{
			String _value;

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

			value = Marshal::ToString(
				_value
			);
		}
	};
}
