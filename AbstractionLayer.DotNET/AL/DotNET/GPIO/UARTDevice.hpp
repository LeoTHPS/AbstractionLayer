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
		// @throw AL::DotNET::Exceptions::Exception
		static void Open([System::Runtime::InteropServices::Out] UARTDevice^% device, System::String^ name, UARTDeviceSpeeds speed)
		{
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

		System::String^ GetName()
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

		// @throw AL::DotNET::Exceptions::Exception
		// @return number of bytes read
		System::UInt32 Read(array<System::Byte>^% buffer, System::UInt32 offset, System::UInt32 count)
		{
			size_t bytesReceived = 0;

			AL::Collections::Array<uint8> _buffer(
				static_cast<size_t>(buffer->Length)
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

			Marshal::Copy(
				&_buffer[0],
				buffer,
				0,
				static_cast<System::UInt32>(bytesReceived)
			);

			return static_cast<System::UInt32>(
				bytesReceived
			);
		}

		// @throw AL::DotNET::Exceptions::Exception
		void Write(array<System::Byte>^ buffer, System::UInt32 offset, System::UInt32 count)
		{
			AL::Collections::Array<uint8> _buffer(
				static_cast<size_t>(buffer->Length)
			);

			Marshal::Copy(
				buffer,
				&_buffer[0],
				0,
				_buffer.GetSize()
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
