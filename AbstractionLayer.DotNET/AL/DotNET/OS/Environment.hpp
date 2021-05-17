#pragma once
#include "AL/DotNET/Common.hpp"

namespace AL::DotNET::OS
{
	public ref class Environment sealed abstract
	{
	public:
		/// <exception cref="AL::Exceptions::Exception" />
		static bool TryGetVariable(::System::String^ name, [::System::Runtime::InteropServices::OutAttribute] ::System::String^% value)
		{
			String _value;

			try
			{
				if (!AL::OS::Environment::TryGetVariable(Marshal::ToNativeString(name), _value))
				{

					return false;
				}
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

			return true;
		}

		/// <exception cref="AL::Exceptions::Exception" />
		static void SetVariable(::System::String^ name, ::System::String^ value)
		{
			try
			{
				AL::OS::Environment::SetVariable(
					Marshal::ToNativeString(name),
					Marshal::ToNativeString(value)
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
