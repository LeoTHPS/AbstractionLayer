#pragma once
#include "AL/Common.hpp"

namespace AL::Graphics
{
	template<typename T>
	using Rectangle_Cast = Safe_Static_Cast<T>;;

	template<typename T, template<typename> typename CAST = Rectangle_Cast>
	struct Rectangle
	{
		T Left;
		T Right;
		T Top;
		T Bottom;

		Rectangle()
			: Rectangle(
				CAST<T>::template Cast<T>(0),
				CAST<T>::template Cast<T>(0)
			)
		{
		}

		Rectangle(T width, T height)
			: Rectangle(
				CAST<T>::template Cast<T>(0),
				width,
				CAST<T>::template Cast<T>(0),
				height
			)
		{
		}

		Rectangle(T left, T right, T top, T bottom)
			: Left(left),
			Right(right),
			Top(top),
			Bottom(bottom)
		{
		}

		virtual ~Rectangle()
		{
		}

		T GetWidth() const
		{
			return Right - Left;
		}

		T GetHeight() const
		{
			return Bottom - Top;
		}

		T GetArea() const
		{
			return GetWidth() * GetHeight();
		}

		T GetPerimeter() const
		{
			return (GetWidth() * 2) + (GetHeight() * 2);
		}

		template<typename _T>
		auto Cast() const
		{
			return Rectangle<_T>(
				CAST<T>::template Cast<_T>(Left),
				CAST<T>::template Cast<_T>(Right),
				CAST<T>::template Cast<_T>(Top),
				CAST<T>::template Cast<_T>(Bottom)
			);
		}

		bool Contains(T x, T y) const
		{
			if ((x < Left) || (x > Right))
				return false;

			if ((y < Top) || (y > Bottom))
				return false;

			return true;
		}

		bool Intersects(const Rectangle& rectangle) const
		{
			return Intersects(
				rectangle.Left,
				rectangle.Right,
				rectangle.Top,
				rectangle.Bottom
			);
		}
		bool Intersects(T left, T right, T top, T bottom) const
		{
			if ((left > Right) || (right < Left))
				return false;

			if ((top < Bottom) || (bottom > Top))
				return false;

			return true;
		}

		bool operator ==(const Rectangle& rectangle) const
		{
			if (Left != rectangle.Left)
				return false;

			if (Right != rectangle.Right)
				return false;

			if (Top != rectangle.Top)
				return false;

			if (Bottom != rectangle.Bottom)
				return false;

			return true;

		}
		bool operator !=(const Rectangle& rectangle) const
		{
			return !operator==(rectangle);
		}
	};
}
