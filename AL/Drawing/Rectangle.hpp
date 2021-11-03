#pragma once
#include "AL/Common.hpp"

namespace AL::Drawing
{
	template<typename T>
	struct Rectangle
	{
		typedef T Type;

		Type Left;
		Type Right;
		Type Top;
		Type Bottom;

		Rectangle()
			: Rectangle(
				Type(),
				Type()
			)
		{
		}

		Rectangle(Type width, Type height)
			: Rectangle(
				Type(),
				width,
				Type(),
				height
			)
		{
		}
		Rectangle(Type left, Type right, Type top, Type bottom)
			: Left(
				left
			),
			Right(
				right
			),
			Top(
				top
			),
			Bottom(
				bottom
			)
		{
		}

		virtual ~Rectangle()
		{
		}

		Type GetWidth() const
		{
			return Right - Left;
		}

		Type GetHeight() const
		{
			return Bottom - Top;
		}

		Type GetArea() const
		{
			return GetWidth() * GetHeight();
		}

		Type GetPerimeter() const
		{
			return (GetWidth() * 2) + (GetHeight() * 2);
		}

		template<typename T_NEW_TYPE>
		auto Cast() const
		{
			return Rectangle<T_NEW_TYPE>(
				BitConverter::Cast<T_NEW_TYPE>(Left),
				BitConverter::Cast<T_NEW_TYPE>(Right),
				BitConverter::Cast<T_NEW_TYPE>(Top),
				BitConverter::Cast<T_NEW_TYPE>(Bottom)
			);
		}

		Bool Contains(Type x, Type y) const
		{
			if ((x < Left) || (x > Right))
			{

				return False;
			}

			if ((y < Top) || (y > Bottom))
			{

				return False;
			}

			return True;
		}

		Bool Intersects(const Rectangle& rectangle) const
		{
			return Intersects(
				rectangle.Left,
				rectangle.Right,
				rectangle.Top,
				rectangle.Bottom
			);
		}
		Bool Intersects(Type left, Type right, Type top, Type bottom) const
		{
			if ((left > Right) || (right < Left))
			{

				return False;
			}

			if ((top < Bottom) || (bottom > Top))
			{

				return False;
			}

			return True;
		}

		Bool operator == (const Rectangle& rectangle) const
		{
			if (Left != rectangle.Left)
			{

				return False;
			}

			if (Right != rectangle.Right)
			{

				return False;
			}

			if (Top != rectangle.Top)
			{

				return False;
			}

			if (Bottom != rectangle.Bottom)
			{

				return False;
			}

			return True;

		}
		Bool operator != (const Rectangle& rectangle) const
		{
			if (operator==(rectangle))
			{

				return False;
			}

			return True;
		}
	};

	typedef Rectangle<int8>    RectangleI8;
	typedef Rectangle<uint8>   RectangleU8;
	
	typedef Rectangle<int16>   RectangleI16;
	typedef Rectangle<uint16>  RectangleU16;
	
	typedef Rectangle<int32>   RectangleI32;
	typedef Rectangle<uint32>  RectangleU32;
	
	typedef Rectangle<int64>   RectangleI64;
	typedef Rectangle<uint64>  RectangleU64;

	typedef Rectangle<ssize_t> RectangleI;
	typedef Rectangle<size_t>  RectangleU;

	typedef Rectangle<Float>   RectangleF;
	typedef Rectangle<Double>  RectangleD;
}
