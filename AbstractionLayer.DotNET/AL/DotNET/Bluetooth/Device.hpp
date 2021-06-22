#pragma once
#include "AL/DotNET/Common.hpp"

#include "Radio.hpp"

#include <AL/Bluetooth/Device.hpp>

namespace AL::DotNET::Bluetooth
{
	ref class Device;

	/// <exception cref="AL::DotNET::Exceptions::Exception" />
	public delegate void DeviceEnumCallback(Device^ device);

	class DeviceEnumNativeCallback
	{
		void* const lpRadio;
		void* const lpCallback;

	public:
		DeviceEnumNativeCallback(Radio^ radio, DeviceEnumCallback^ callback)
			: lpRadio(
				::System::Runtime::InteropServices::GCHandle::ToIntPtr(
					Marshal::GetHandle(radio)
				).ToPointer()
			),
			lpCallback(
				::System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(
					callback
				).ToPointer()
			)
		{
		}

		virtual ~DeviceEnumNativeCallback()
		{
			auto hRadio = ::System::Runtime::InteropServices::GCHandle::FromIntPtr(
				::System::IntPtr(lpRadio)
			);

			Marshal::FreeHandle(
				hRadio
			);
		}

		// @throw AL::Exceptions::Exception
		void Execute(AL::Bluetooth::Device& device) const;
	};

	public ref class Device
	{
		Radio^ radio;

	internal:
		AL::Bluetooth::Device* const lpDevice;

		Device(Radio^ radio, AL::Bluetooth::Device&& device)
			: radio(
				radio
			),
			lpDevice(
				new AL::Bluetooth::Device(
					Move(device)
				)
			)
		{
		}

	public:
		/// <exception cref="AL::DotNET::Exceptions::Exception" />
		static void Enumerate(Radio^ radio, DeviceEnumCallback^ callback)
		{
			::System::GC::KeepAlive(
				radio
			);
			
			::System::GC::KeepAlive(
				callback
			);

			DeviceEnumNativeCallback nativeCallback(
				radio,
				callback
			);

			try
			{
				AL::Bluetooth::Device::Enumerate(
					*radio->lpRadio,
					AL::Bluetooth::DeviceEnumCallback(
						&DeviceEnumNativeCallback::Execute,
						nativeCallback
					)
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}
		}

		virtual ~Device()
		{
			delete lpDevice;
		}

		bool IsOpen()
		{
			return lpDevice->IsOpen();
		}

		bool IsConnected()
		{
			return lpDevice->IsOpen();
		}

		bool IsRemembered()
		{
			return lpDevice->IsRemembered();
		}

		bool IsAuthenticated()
		{
			return lpDevice->IsAuthenticated();
		}

		Radio^ GetRadio()
		{
			return radio;
		}

		::System::String^ GetName()
		{
			return Marshal::ToString(
				lpDevice->GetName()
			);
		}

		::System::UInt64 GetAddress()
		{
			return lpDevice->GetAddress();
		}

		::System::UInt64 GetDeviceClass()
		{
			return lpDevice->GetDeviceClass();
		}

		::System::DateTime GetLastSeenTime()
		{
			auto dateTime = lpDevice->GetLastSeenTime();

			return ::System::DateTime(
				static_cast<System::Int32>(dateTime.Year),
				static_cast<System::Int32>(dateTime.Month),
				static_cast<System::Int32>(dateTime.Day),
				static_cast<System::Int32>(dateTime.Hour),
				static_cast<System::Int32>(dateTime.Minutes),
				static_cast<System::Int32>(dateTime.Seconds),
				static_cast<System::Int32>(dateTime.Milliseconds)
			);
		}

		::System::DateTime GetLastUsedTime()
		{
			auto dateTime = lpDevice->GetLastSeenTime();

			return ::System::DateTime(
				static_cast<System::Int32>(dateTime.Year),
				static_cast<System::Int32>(dateTime.Month),
				static_cast<System::Int32>(dateTime.Day),
				static_cast<System::Int32>(dateTime.Hour),
				static_cast<System::Int32>(dateTime.Minutes),
				static_cast<System::Int32>(dateTime.Seconds),
				static_cast<System::Int32>(dateTime.Milliseconds)
			);
		}

		void Close()
		{
			lpDevice->Close();
		}
	};
}

// @throw AL::Exceptions::Exception
inline void AL::DotNET::Bluetooth::DeviceEnumNativeCallback::Execute(AL::Bluetooth::Device& device) const
{
	auto hRadio = ::System::Runtime::InteropServices::GCHandle::FromIntPtr(
		::System::IntPtr(lpRadio)
	);

	auto radio = static_cast<Radio^>(
		hRadio.Target
	);

	auto callback = ::System::Runtime::InteropServices::Marshal::GetDelegateForFunctionPointer<DeviceEnumCallback^>(
		::System::IntPtr(lpCallback)
	);

	try
	{
		callback(
			gcnew Device(
				radio,
				Move(device)
			)
		);
	}
	catch (::System::Exception^ exception)
	{

		throw Marshal::ToNativeException(
			exception
		);
	}
}
