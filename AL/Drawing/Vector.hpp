#pragma once
#include "AL/Common.hpp"

#include <math.h>

namespace AL::Drawing
{
	template<size_t D, typename T>
	struct Vector;

	template<typename T>
	struct Vector<1, T>
	{
		typedef T Type;

		Type X;

		Vector()
		{
		}

		Vector(Type x)
			: X(
				x
			)
		{
		}

		template<typename T_NEW_TYPE>
		auto Cast() const
		{
			return Vector<1, T_NEW_TYPE>(
				BitConverter::Cast<T_NEW_TYPE>(X)
			);
		}

		auto GetDistance(Type x) const
		{
			return (x > X) ? (x - X) : (X - x);
		}
		auto GetDistance(const Vector& vector) const
		{
			return GetDistance(
				vector.X
			);
		}

		Bool operator == (const Vector& vector) const
		{
			if (X != vector.X)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const Vector& vector) const
		{
			if (operator==(vector))
			{

				return False;
			}

			return True;
		}
	};

	template<typename T>
	struct Vector<2, T>
		: public Vector<1, T>
	{
		typedef T Type;

		Type Y;

		Vector()
		{
		}

		Vector(Type x, Type y)
			: Vector<1, T>(
				x
			),
			Y(
				y
			)
		{
		}

		template<typename T_NEW_TYPE>
		auto Cast() const
		{
			return Vector<2, T_NEW_TYPE>(
				BitConverter::Cast<T_NEW_TYPE>(Vector<1, T>::X),
				BitConverter::Cast<T_NEW_TYPE>(Y)
			);
		}

		Bool IsParallel(Type x, Type y) const
		{
			return (Vector<1, T>::X == x) ^ (Y == y);
		}
		Bool IsParallel(const Vector& vector) const
		{
			if (!IsParallel(vector.X, vector.Y))
			{

				return False;
			}

			return True;
		}

		auto GetAngle(Type x, Type y) const
		{
			return static_cast<Float>(
				::atan2(y - Y, x - Vector<1, T>::X) * Math::Rad2Deg
			);
		}
		auto GetAngle(const Vector& vector) const
		{
			return GetAngle(
				vector.X,
				vector.Y
			);
		}

		auto GetDistance(Type x, Type y) const
		{
			return (Vector<1, T>::X >= x ? Vector<1, T>::X - x : x - Vector<1, T>::X) + (Y >= y ? Y - y : y - Y);
		}
		auto GetDistance(const Vector& vector) const
		{
			return GetDistance(
				vector.X,
				vector.Y
			);
		}

		Bool operator == (const Vector& vector) const
		{
			if (!Vector<1, Type>::operator==(vector))
			{

				return False;
			}

			if (Y != vector.Y)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const Vector& vector) const
		{
			if (operator==(vector))
			{

				return False;
			}

			return True;
		}
	};

	template<typename T>
	struct Vector<3, T>
		: public Vector<2, T>
	{
		typedef T Type;

		Type Z;

		Vector()
		{
		}

		Vector(Type x, Type y, Type z)
			: Vector<2, T>(
				x,
				y
			),
			Z(
				z
			)
		{
		}

		template<typename T_NEW_TYPE>
		auto Cast() const
		{
			return Vector<3, T_NEW_TYPE>(
				BitConverter::Cast<T_NEW_TYPE>(Vector<2, T>::X),
				BitConverter::Cast<T_NEW_TYPE>(Vector<2, T>::Y),
				BitConverter::Cast<T_NEW_TYPE>(Z)
			);
		}

		auto GetDistance(Type x, Type y, Type z) const
		{
			auto distance = Vector<2, T>::GetDistance(
				x,
				y
			);

			return (Z >= z) ? Z - z : z - Z;
		}
		auto GetDistance(const Vector& vector) const
		{
			return GetDistance(
				vector.X,
				vector.Y,
				vector.Z
			);
		}

		Bool operator == (const Vector& vector) const
		{
			if (!Vector<2, Type>::operator==(vector))
			{

				return False;
			}

			if (Z != vector.Z)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const Vector& vector) const
		{
			if (operator==(vector))
			{

				return False;
			}

			return True;
		}
	};

	template<typename T>
	struct Vector<4, T>
		: public Vector<3, T>
	{
		typedef T Type;

		Type R;

		Vector()
		{
		}

		Vector(Type x, Type y, Type z, Type r)
			: Vector<3, T>(
				x,
				y,
				z
			),
			R(
				r
			)
		{
		}

		template<typename T_NEW_TYPE>
		auto Cast() const
		{
			return Vector<3, T_NEW_TYPE>(
				BitConverter::Cast<T_NEW_TYPE>(Vector<3, T>::X),
				BitConverter::Cast<T_NEW_TYPE>(Vector<3, T>::Y),
				BitConverter::Cast<T_NEW_TYPE>(Vector<3, T>::Z),
				BitConverter::Cast<T_NEW_TYPE>(R)
			);
		}

		Bool IsFacing(Type x, Type y, Float fieldOfView) const
		{
			auto angle = GetAngle(
				x,
				y
			);

			if (angle >= 0)
			{

				angle -= 90;
			}
			else if (angle < 0)
			{

				angle += 270;
			}

			if (angle < -180)
			{

				angle += 360;
			}

			float fieldDelta = 0;
			float fieldLeft = R - (fieldOfView / 2);
			float fieldRight = R + (fieldOfView / 2);

			if (fieldLeft < 0)
			{

				fieldDelta = -fieldLeft;
			}
			else if (fieldRight > 360)
			{

				fieldDelta = -(fieldRight - 360);
			}

			fieldLeft += fieldDelta;
			fieldRight += fieldDelta;

			if ((angle += fieldDelta) < 0)
			{

				angle += 360;
			}
			else if (angle > 360)
			{

				angle -= 360;
			}

			return (angle >= fieldLeft) && (angle <= fieldRight);
		}
		Bool IsFacing(const Vector& vector, Float fieldOfView) const
		{
			if (!IsFacing(vector.X, vector.Y, vector.Z, fieldOfView))
			{

				return False;
			}

			return True;
		}

		Bool operator == (const Vector& vector) const
		{
			if (!Vector<3, Type>::operator==(vector))
			{

				return False;
			}

			if (R != vector.R)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const Vector& vector) const
		{
			if (operator==(vector))
			{

				return False;
			}

			return True;
		}
	};

	template<size_t D>
	using VectorI8 = Vector<D, int8>;
	template<size_t D>
	using VectorU8 = Vector<D, uint8>;

	template<size_t D>
	using VectorI16 = Vector<D, int16>;
	template<size_t D>
	using VectorU16 = Vector<D, uint16>;

	template<size_t D>
	using VectorI32 = Vector<D, int32>;
	template<size_t D>
	using VectorU32 = Vector<D, uint32>;

	template<size_t D>
	using VectorI64 = Vector<D, int64>;
	template<size_t D>
	using VectorU64 = Vector<D, uint64>;

	template<size_t D>
	using VectorF = Vector<D, Float>;
	template<size_t D>
	using VectorD = Vector<D, Double>;

	template<typename T>
	using Vector1    = Vector<1, T>;
	using Vector1I8  = VectorI8<1>;
	using Vector1U8  = VectorU8<1>;
	using Vector1I16 = VectorI16<1>;
	using Vector1U16 = VectorU16<1>;
	using Vector1I32 = VectorI32<1>;
	using Vector1U32 = VectorU32<1>;
	using Vector1I64 = VectorI64<1>;
	using Vector1U64 = VectorU64<1>;
	using Vector1F   = VectorF<1>;
	using Vector1D   = VectorD<1>;

	template<typename T>
	using Vector2    = Vector<2, T>;
	using Vector2I8  = VectorI8<2>;
	using Vector2U8  = VectorU8<2>;
	using Vector2I16 = VectorI16<2>;
	using Vector2U16 = VectorU16<2>;
	using Vector2I32 = VectorI32<2>;
	using Vector2U32 = VectorU32<2>;
	using Vector2I64 = VectorI64<2>;
	using Vector2U64 = VectorU64<2>;
	using Vector2F   = VectorF<2>;
	using Vector2D   = VectorD<2>;

	template<typename T>
	using Vector3    = Vector<3, T>;
	using Vector3I8  = VectorI8<3>;
	using Vector3U8  = VectorU8<3>;
	using Vector3I16 = VectorI16<3>;
	using Vector3U16 = VectorU16<3>;
	using Vector3I32 = VectorI32<3>;
	using Vector3U32 = VectorU32<3>;
	using Vector3I64 = VectorI64<3>;
	using Vector3U64 = VectorU64<3>;
	using Vector3F   = VectorF<3>;
	using Vector3D   = VectorD<3>;

	template<typename T>
	using Vector4    = Vector<4, T>;
	using Vector4I8  = VectorI8<4>;
	using Vector4U8  = VectorU8<4>;
	using Vector4I16 = VectorI16<4>;
	using Vector4U16 = VectorU16<4>;
	using Vector4I32 = VectorI32<4>;
	using Vector4U32 = VectorU32<4>;
	using Vector4I64 = VectorI64<4>;
	using Vector4U64 = VectorU64<4>;
	using Vector4F   = VectorF<4>;
	using Vector4D   = VectorD<4>;
}
