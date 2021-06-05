#pragma once
#include "AL/DotNET/Common.hpp"

#include <AL/Algorithms/CRC.hpp>

namespace AL::DotNET::Algorithms
{
	public ref class CRC16
	{
		CRC16()
		{
		}

	public:
		static ::System::UInt16 Calculate(::System::String^ string)
		{
			::System::UInt16 crc = AL::Algorithms::_CRC_Constants<uint16>::CRC;

			for (::System::Int32 index = 0; index < string->Length; ++index)
			{
				for (size_t b = 0; b < 8; ++b)
				{
					if (((static_cast<::System::Byte>(string[index]) >> (7 - b) & 1) == 1) ^ ((crc >> 15 & 1) == 1))
					{
						crc <<= 1;
						crc ^= AL::Algorithms::_CRC_Constants<uint16>::POLYNOMIAL;
					}
					else
					{
						crc <<= 1;
					}
				}
			}

			return crc & 0xFFFF;
		}

		static ::System::UInt16 Calculate(array<::System::Byte>^ array, ::System::Int32 index, ::System::Int32 count)
		{
			::System::UInt16 crc = AL::Algorithms::_CRC_Constants<uint16>::CRC;

			for (::System::Int32 i = 0; i < count; ++i, ++index)
			{
				for (size_t b = 0; b < 8; ++b)
				{
					if (((array[index] >> (7 - b) & 1) == 1) ^ ((crc >> 15 & 1) == 1))
					{
						crc <<= 1;
						crc ^= AL::Algorithms::_CRC_Constants<uint16>::POLYNOMIAL;
					}
					else
					{
						crc <<= 1;
					}
				}
			}

			return crc & 0xFFFF;
		}
	};
	
	public ref class CRC32
	{
		CRC32()
		{
		}

	public:
		static ::System::UInt32 Calculate(::System::String^ string)
		{
			::System::UInt32 crc = AL::Algorithms::_CRC_Constants<uint32>::CRC;

			for (::System::Int32 index = 0; index < string->Length; ++index)
			{
				crc = AL::Algorithms::_CRC_Constants<uint32>::Table[(crc ^ static_cast<uint32>(string[index])) & 0xFF] ^ (crc >> 8);
			}

			return ~crc;
		}

		static ::System::UInt32 Calculate(array<::System::Byte>^ array, ::System::Int32 index, ::System::Int32 count)
		{
			::System::UInt32 crc = AL::Algorithms::_CRC_Constants<uint32>::CRC;

			for (::System::Int32 i = 0; i < count; ++i, ++index)
			{
				crc = AL::Algorithms::_CRC_Constants<uint32>::Table[(crc ^ static_cast<uint32>(array[index])) & 0xFF] ^ (crc >> 8);
			}

			return ~crc;
		}
	};
}
