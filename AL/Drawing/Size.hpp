#pragma once
#include "AL/Common.hpp"

namespace AL::Drawing
{
	template<typename T>
	struct Size
	{
		typedef T Type;

		Type Width;
		Type Height;

		Size()
			: Size(
				Type(),
				Type()
			)
		{
		}

		Size(Type width, Type height)
			: Width(
				width
			),
			Height(
				height
			)
		{
		}

		template<typename T_NEW_TYPE>
		auto Cast() const
		{
			return Size<T_NEW_TYPE>(
				BitConverter::Cast<T_NEW_TYPE>(Width),
				BitConverter::Cast<T_NEW_TYPE>(Height)
			);
		}

		Bool operator == (const Size& size) const
		{
			if (Width != size.Width)
			{

				return False;
			}

			if (Height != size.Height)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const Size& size) const
		{
			if (operator==(size))
			{

				return False;
			}

			return True;
		}
	};

	typedef Size<ssize_t> SizeI;
	typedef Size<size_t>  SizeU;

	typedef Size<int8>    SizeI8;
	typedef Size<uint8>   SizeU8;

	typedef Size<int16>   SizeI16;
	typedef Size<uint16>  SizeU16;

	typedef Size<int32>   SizeI32;
	typedef Size<uint32>  SizeU32;

	typedef Size<int64>   SizeI64;
	typedef Size<uint64>  SizeU64;

	typedef Size<Float>   SizeF;
	typedef Size<Double>  SizeD;
}
