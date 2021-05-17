#pragma once
#include "AL/DotNET/Common.hpp"

namespace AL::DotNET::OS
{
	public ref class System sealed abstract
	{
	public:
		static auto GetTimezone()
		{
			return AL::OS::System::GetTimezone();
		}

		static auto GetDateTime()
		{
			auto dateTime = AL::OS::System::GetDateTime();

			return ::System::DateTime(
				static_cast<::System::Int32>(dateTime.Year),
				static_cast<::System::Int32>(dateTime.Month),
				static_cast<::System::Int32>(dateTime.Day),
				static_cast<::System::Int32>(dateTime.Hour),
				static_cast<::System::Int32>(dateTime.Minutes),
				static_cast<::System::Int32>(dateTime.Seconds),
				static_cast<::System::Int32>(dateTime.Milliseconds)
			);
		}

		static auto GetTimestamp()
		{
			auto timestamp = AL::OS::System::GetTimestamp();
			auto milliseconds = timestamp.ToMilliseconds();
			auto millisecondFractionInUS = timestamp.ToMicroseconds() - (milliseconds * 1000);

			return ::System::TimeSpan::FromMilliseconds(
				(milliseconds * 1.0) + (millisecondFractionInUS / 1000.0)
			);
		}

		static auto GetPageSize()
		{
			return AL::OS::System::GetPageSize();
		}

		static auto GetMinimumAddress()
		{
			return AL::OS::System::GetMinimumAddress();
		}

		static auto GetMaximumAddress()
		{
			return AL::OS::System::GetMaximumAddress();
		}

		static auto GetProcessorCount()
		{
			return AL::OS::System::GetProcessorCount();
		}

		/// <exception cref="AL::Exceptions::Exception" />
		static auto GetProcessorCacheCount()
		{
			try
			{
				return AL::OS::System::GetProcessorCacheCount();
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}
		}

		/// <exception cref="AL::Exceptions::Exception" />
		static auto GetProcessorCacheSize(::System::UInt32 cacheLevel)
		{
			try
			{
				return AL::OS::System::GetProcessorCacheSize(
					cacheLevel
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}
		}

		/// <exception cref="AL::Exceptions::Exception" />
		static auto GetProcessorCacheLineSize(::System::UInt32 cacheLevel)
		{
			try
			{
				return AL::OS::System::GetProcessorCacheLineSize(
					cacheLevel
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}
		}
	};
}
