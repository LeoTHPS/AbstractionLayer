#pragma once
#include "AL/DotNET/Common.hpp"

#include <AL/Algorithms/FNV.hpp>

namespace AL::DotNET::Algorithms
{
	public ref class FNV32
	{
		FNV32()
		{
		}

	public:
		static System::UInt32 Calculate(System::String^ string)
		{
			System::UInt32 hash = AL::Algorithms::_FNV_Constants<uint32>::Offset;

			for (System::Int32 index = 0; index < string->Length; ++index)
			{
				hash ^= static_cast<System::UInt32>(
					static_cast<System::Byte>(string[index])
				);

				hash *= AL::Algorithms::_FNV_Constants<uint32>::Prime;
			}

			return hash;
		}

		static System::UInt32 Calculate(array<System::Byte>^ array, System::Int32 index, System::Int32 count)
		{
			System::UInt32 hash = AL::Algorithms::_FNV_Constants<uint32>::Offset;

			for (; index < count; ++index)
			{
				hash ^= static_cast<System::UInt32>(
					array[index]
				);

				hash *= AL::Algorithms::_FNV_Constants<uint32>::Prime;
			}

			return hash;
		}
	};

	public ref class FNV64
	{
		FNV64()
		{
		}

	public:
		static System::UInt64 Calculate(System::String^ string)
		{
			System::UInt64 hash = AL::Algorithms::_FNV_Constants<uint64>::Offset;

			for (System::Int32 index = 0; index < string->Length; ++index)
			{
				hash ^= static_cast<System::UInt64>(
					static_cast<System::Byte>(string[index])
				);

				hash *= AL::Algorithms::_FNV_Constants<uint64>::Prime;
			}

			return hash;
		}

		static System::UInt64 Calculate(array<System::Byte>^ array, System::Int32 index, System::Int32 count)
		{
			System::UInt64 hash = AL::Algorithms::_FNV_Constants<uint64>::Offset;

			for (; index < count; ++index)
			{
				hash ^= static_cast<System::UInt64>(
					array[index]
				);

				hash *= AL::Algorithms::_FNV_Constants<uint64>::Prime;
			}

			return hash;
		}
	};
}
