#pragma once
#include "AL/Common.hpp"

namespace AL
{
	class HexConverter
	{
		HexConverter() = delete;

	public:
		typedef typename BaseConverter::Buffer Buffer;

		template<typename T>
		static String Encode(const T& value)
		{
			return BaseConverter::ToBase16(
				value
			);
		}
		static String Encode(const Void* lpBuffer, size_t size)
		{
			return BaseConverter::ToBase16(
				lpBuffer,
				size
			);
		}

		template<typename T>
		static T    Decode(const String& string)
		{
			return BaseConverter::FromBase16<T>(
				string
			);
		}
		static Bool Decode(Buffer& buffer, const String& string)
		{
			if (!BaseConverter::FromBase16(buffer, string))
			{

				return False;
			}

			return True;
		}
	};
}
