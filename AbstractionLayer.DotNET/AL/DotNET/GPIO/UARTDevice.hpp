#pragma once
#include "AL/DotNET/Common.hpp"

#include "UARTDeviceFlags.hpp"
#include "UARTDeviceSpeeds.hpp"

#include <AL/GPIO/UARTDevice.hpp>

#include <AL/Collections/Array.hpp>

namespace AL::DotNET::GPIO
{
	public ref class UARTDevice
	{
		AL::GPIO::UARTDevice* const lpDevice;

		explicit UARTDevice(AL::GPIO::UARTDevice&& device)
			: lpDevice(
				new AL::GPIO::UARTDevice(
					Move(device)
				)
			)
		{
		}

	public:
		/// <exception cref="AL::DotNET::Exceptions::Exception" />
		static void Open([::System::Runtime::InteropServices::Out] UARTDevice^% device, ::System::String^ name, UARTDeviceSpeeds speed)
		{
			Open(
				device,
				name,
				speed,
				UARTDeviceFlags::None
			);
		}
		/// <exception cref="AL::DotNET::Exceptions::Exception" />
		static void Open([::System::Runtime::InteropServices::Out] UARTDevice^% device, ::System::String^ name, UARTDeviceSpeeds speed, UARTDeviceFlags flags)
		{
			AL::GPIO::UARTDevice _device;

			try
			{
				AL::GPIO::UARTDevice::Open(
					_device,
					Marshal::ToNativeString(name),
					static_cast<AL::GPIO::UARTDeviceSpeeds>(speed),
					static_cast<AL::GPIO::UARTDeviceFlags>(flags)
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}

			device = gcnew UARTDevice(
				Move(_device)
			);
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

		/// <exception cref="AL::DotNET::Exceptions::Exception" />
		void Read(array<::System::Byte>^% buffer, ::System::UInt32 offset, ::System::UInt32 count)
		{
			AL::Collections::Array<uint8> _buffer(
				count
			);

			try
			{
				lpDevice->Read(
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
				static_cast<::System::Int32>(count)
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
