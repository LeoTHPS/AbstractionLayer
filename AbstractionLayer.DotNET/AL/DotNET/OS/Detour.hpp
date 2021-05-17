#pragma once
#include "AL/DotNET/Common.hpp"

namespace AL::DotNET::OS
{
	public ref class Detour sealed abstract
	{
	public:
		/// <exception cref="AL::Exceptions::Exception" />
		static void HookFunction(::System::IntPtr source, ::System::IntPtr destination)
		{
			try
			{
				AL::OS::Detour::HookFunction(
					source.ToPointer(),
					destination.ToPointer()
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
