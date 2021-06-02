#pragma once
#include "AL/DotNET/Common.hpp"

#include <AL/Algorithms/Sha1.hpp>

#include <AL/Collections/Array.hpp>

namespace AL::DotNET::Algorithms
{
	public ref class Sha1
	{
		AL::Algorithms::Sha1* const lpSha1;

	public:
		Sha1()
			: lpSha1(
				new AL::Algorithms::Sha1()
			)
		{
			Reset();
		}

		virtual ~Sha1()
		{
			delete lpSha1;
		}

		void Reset()
		{
			lpSha1->Reset();
		}

		bool Add(array<::System::Byte>^% buffer, ::System::UInt32 offset, ::System::UInt32 count)
		{
			AL::Collections::Array<uint8> _buffer(
				count
			);

			_buffer.SetSize(
				count
			);

			Marshal::Copy(
				buffer,
				&_buffer[0],
				offset,
				count
			);

			return lpSha1->Add(
				&_buffer[0],
				_buffer.GetCapacity()
			);
		}

		bool Calculate([::System::Runtime::InteropServices::OutAttribute] ::System::String^% result)
		{
			AL::Algorithms::Sha1::Hash hash;

			if (!lpSha1->Calculate(hash))
			{

				return false;
			}

			result = gcnew ::System::String(
				""
			);

			for (size_t i = 0; i < hash.GetSize(); ++i)
			{
				result += ::System::String::Format(
					"{0:X2}",
					hash[i]
				);
			}

			return true;
		}
		bool Calculate([::System::Runtime::InteropServices::OutAttribute] array<::System::Byte>^% buffer)
		{
			AL::Algorithms::Sha1::Hash hash;

			if (!lpSha1->Calculate(hash))
			{

				return false;
			}

			buffer = gcnew array<::System::Byte>(
				hash.GetSize()
			);

			Marshal::Copy(
				&hash[0],
				buffer,
				0,
				static_cast<::System::UInt32>(hash.GetSize())
			);

			return true;
		}

		static bool Calculate([::System::Runtime::InteropServices::OutAttribute] ::System::String^% result, array<::System::Byte>^ buffer, ::System::UInt32 offset, ::System::UInt32 count)
		{
			Sha1 sha1;

			if (!sha1.Add(buffer, 0, buffer->Length))
			{

				return false;
			}

			if (!sha1.Calculate(result))
			{

				return false;
			}

			return true;
		}
		static bool Calculate([::System::Runtime::InteropServices::OutAttribute] array<::System::Byte>^% result, array<::System::Byte>^ buffer, ::System::UInt32 offset, ::System::UInt32 count)
		{
			Sha1 sha1;

			if (!sha1.Add(buffer, 0, buffer->Length))
			{

				return false;
			}

			if (!sha1.Calculate(result))
			{

				return false;
			}

			return true;
		}
	};
}
