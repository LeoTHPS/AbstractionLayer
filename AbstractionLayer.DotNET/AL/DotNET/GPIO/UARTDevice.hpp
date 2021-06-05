#pragma once
#include "AL/DotNET/Common.hpp"

#include "UARTDeviceSpeeds.hpp"

#include <AL/GPIO/UARTDevice.hpp>

#include <AL/Collections/Array.hpp>

namespace AL::DotNET::GPIO
{
	public ref class UARTDevice
	{
		AL::GPIO::UARTDevice* const lpDevice;

	public:
		/// <exception cref="AL::DotNET::Exceptions::Exception" />
		static void Open([::System::Runtime::InteropServices::Out] UARTDevice^% device, ::System::String^ name, UARTDeviceSpeeds speed)
		{
			device = gcnew UARTDevice();

			try
			{
				AL::GPIO::UARTDevice::Open(
					*device->lpDevice,
					Marshal::ToNativeString(name),
					static_cast<AL::GPIO::UARTDeviceSpeeds>(speed)
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{
				delete device;

				throw gcnew Exceptions::Exception(
					exception
				);
			}
		}

		UARTDevice()
			: lpDevice(
				new AL::GPIO::UARTDevice()
			)
		{
		}

		virtual ~UARTDevice()
		{
			delete lpDevice;
		}

		bool IsOpen()
		{
			return lpDevice->IsOpen();
		}

		::System::String^ GetName()
		{
			return Marshal::ToString(
				lpDevice->GetName()
			);
		}

		UARTDeviceSpeeds GetSpeed()
		{
			return static_cast<UARTDeviceSpeeds>(
				lpDevice->GetSpeed()
			);
		}

		/// <summary>
		/// Returns number of bytes read
		/// </summary>
		/// <exception cref="AL::DotNET::Exceptions::Exception" />
		::System::UInt32 Read(array<::System::Byte>^% buffer, ::System::UInt32 offset, ::System::UInt32 count)
		{
			size_t bytesReceived = 0;

			AL::Collections::Array<uint8> _buffer(
				count
			);

			try
			{
				bytesReceived = lpDevice->Read(
					&_buffer[0],
					_buffer.GetSize()
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}

			::System::Runtime::InteropServices::Marshal::Copy(
				::System::IntPtr(&_buffer[0]),
				buffer,
				static_cast<::System::Int32>(offset),
				static_cast<::System::Int32>(bytesReceived)
			);

			return static_cast<System::UInt32>(
				bytesReceived
			);
		}

		/// <exception cref="AL::DotNET::Exceptions::Exception" />
		void Write(array<::System::Byte>^ buffer, ::System::UInt32 offset, ::System::UInt32 count)
		{
			AL::Collections::Array<uint8> _buffer(
				count
			);

			::System::Runtime::InteropServices::Marshal::Copy(
				buffer,
				static_cast<::System::Int32>(offset),
				::System::IntPtr(&_buffer[0]),
				static_cast<::System::Int32>(count)
			);

			try
			{
				lpDevice->Write(
					&_buffer[0],
					_buffer.GetSize()
				);
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
	};
}
