#pragma once
#include "AL/DotNET/Common.hpp"

#include <AL/Bluetooth/Radio.hpp>

namespace AL::DotNET::Bluetooth
{
	ref class Radio;

	/// <exception cref="AL::DotNET::Exceptions::Exception" />
	public delegate void RadioEnumCallback(Radio^ radio);

	class RadioEnumCallbackNativeCallback
	{
		void* const lpCallback;

	public:
		explicit RadioEnumCallbackNativeCallback(RadioEnumCallback^ callback)
			: lpCallback(
				::System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(
					callback
				).ToPointer()
			)
		{
		}

		virtual ~RadioEnumCallbackNativeCallback()
		{
		}

		// @throw AL::Exceptions::Exception
		void Execute(AL::Bluetooth::Radio& radio) const;
	};

	public ref class Radio
	{
	internal:
		AL::Bluetooth::Radio* const lpRadio;

		explicit Radio(AL::Bluetooth::Radio&& radio)
			: lpRadio(
				new AL::Bluetooth::Radio(
					Move(radio)
				)
			)
		{
		}

	public:
		/// <exception cref="AL::DotNET::Exceptions::Exception" />
		static void Enumerate(RadioEnumCallback^ callback)
		{
			::System::GC::KeepAlive(
				callback
			);

			RadioEnumCallbackNativeCallback nativeCallback(
				callback
			);

			try
			{
				AL::Bluetooth::Radio::Enumerate(
					AL::Bluetooth::RadioEnumCallback(
						&RadioEnumCallbackNativeCallback::Execute,
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

		virtual ~Radio()
		{
			delete lpRadio;
		}

		::System::String^ GetName()
		{
			return Marshal::ToString(
				lpRadio->GetName()
			);
		}

		::System::UInt64 GetAddress()
		{
			return lpRadio->GetAddress();
		}

		::System::UInt64 GetDeviceClass()
		{
			return lpRadio->GetDeviceClass();
		}

		::System::UInt16 GetSubVersion()
		{
			return lpRadio->GetSubVersion();
		}

		::System::UInt16 GetManufacturer()
		{
			return lpRadio->GetManufacturer();
		}

		void Close()
		{
			lpRadio->Close();
		}
	};
}

// @throw AL::Exceptions::Exception
inline void AL::DotNET::Bluetooth::RadioEnumCallbackNativeCallback::Execute(AL::Bluetooth::Radio& radio) const
{
	auto callback = ::System::Runtime::InteropServices::Marshal::GetDelegateForFunctionPointer<RadioEnumCallback^>(
		::System::IntPtr(lpCallback)
	);

	try
	{
		callback(
			gcnew Radio(
				Move(radio)
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
