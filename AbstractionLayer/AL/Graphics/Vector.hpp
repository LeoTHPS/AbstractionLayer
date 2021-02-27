#pragma once
#include "AL/Common.hpp"

namespace AL::Graphics
{
	template<typename T, size_t POINT_COUNT, template<typename> typename CAST>
	struct _Vector;

	template<typename T>
	using Vector_Cast = Safe_Static_Cast<T>;

	template<typename T, template<typename> typename CAST>
	struct _Vector<T, 2, CAST>
	{
		typedef T Type;

		T X;
		T Y;

		_Vector()
			: _Vector(
				CAST<T>::template Cast<T>(0),
				CAST<T>::template Cast<T>(0)
			)
		{
		}

		_Vector(T x, T y)
			: X(x),
			Y(y)
		{
		}

		virtual ~_Vector()
		{
		}

		bool IsParallel(T x, T y) const
		{
			return (X == x) ^ (Y == y);
		}
		bool IsParallel(const _Vector& vector) const
		{
			return IsParallel(
				vector.X,
				vector.Y
			);
		}

		float GetAngle(T x, T y) const
		{
			_Vector direction(
				x - X,
				y - Y
			);

			return static_cast<float>(
				::atan2(direction.Y, direction.X) * Math::Rad2Deg
			);
		}
		float GetAngle(const _Vector& vector) const
		{
			return GetAngle(
				vector.X,
				vector.Y
			);
		}

		template<typename _T>
		auto Cast() const
		{
			return _Vector<_T, 2, CAST>(
				CAST<T>::template Cast<_T>(X),
				CAST<T>::template Cast<_T>(Y)
			);
		}

		T GetDistance(T x, T y) const
		{
			_Vector distance(
				X >= x ? X - x : x - X,
				Y >= y ? Y - y : y - Y
			);

			return distance.X + distance.Y;
		}
		T GetDistance(const _Vector& vector) const
		{
			return GetDistance(
				vector.X,
				vector.Y
			);
		}

		bool operator ==(const _Vector& vector) const
		{
			if (X != vector.X)
				return false;

			if (Y != vector.Y)
				return false;

			return true;
		}
		bool operator !=(const _Vector& vector) const
		{
			return !operator==(vector);
		}
	};

	template<typename T, template<typename> typename CAST>
	using _Vector2 = _Vector<T, 2, CAST>;

	template<typename T, template<typename> typename CAST>
	struct _Vector<T, 3, CAST>
		: public _Vector2<T, CAST>
	{
		typedef T Type;

		T Z;

		_Vector()
			: _Vector2<T, CAST>()
		{
		}

		_Vector(T x, T y, T z)
			: _Vector2<T, CAST>(x, y),
			Z(z)
		{
		}

		template<typename _T>
		auto Cast() const
		{
			return _Vector<_T, 3, CAST>(
				CAST<T>::template Cast<_T>(
					_Vector2<T, CAST>::X
				),
				CAST<T>::template Cast<_T>(
					_Vector2<T, CAST>::Y
				),
				CAST<T>::template Cast<_T>(Z)
			);
		}

		T GetDistance(T x, T y, T z) const
		{
			return _Vector2<T, CAST>::GetDistance(
				x,
				y
			) + (Z >= z) ? Z - z : z - Z;
		}
		T GetDistance(const _Vector& vector) const
		{
			return GetDistance(
				vector.X,
				vector.Y,
				vector.Z
			);
		}

		bool operator ==(const _Vector& vector) const
		{
			if (Z != vector.Z)
				return false;

			return _Vector2<T, CAST>::operator==(vector);
		}
		bool operator !=(const _Vector& vector) const
		{
			return !operator==(vector);
		}
	};

	template<typename T, template<typename> typename CAST>
	using _Vector3 = _Vector<T, 3, CAST>;

	template<typename T, template<typename> typename CAST>
	struct _Vector<T, 4, CAST>
		: public _Vector3<T, CAST>
	{
		typedef T Type;

		T R;

		_Vector()
			: _Vector3<T, CAST>()
		{
		}

		_Vector(T x, T y, T z, T r)
			: _Vector3<T, CAST>(x, y, z),
			R(r)
		{
		}

		bool IsFacing(T x, T y, T z, T r, float fieldOfView) const
		{
			auto angle = GetAngle(x, y);

			if (angle >= 0)
				angle -= 90;

			else if (angle < 0)
				angle += 270;

			if (angle < -180)
				angle += 360;

			float fieldDelta = 0;
			float fieldLeft = R - (fieldOfView / 2);
			float fieldRight = R + (fieldOfView / 2);

			if (fieldLeft < 0)
				fieldDelta = -fieldLeft;

			else if (fieldRight > 360)
				fieldDelta = -(fieldRight - 360);

			fieldLeft += fieldDelta;
			fieldRight += fieldDelta;

			if ((angle += fieldDelta) < 0)
				angle += 360;

			else if (angle > 360)
				angle -= 360;

			return (angle >= fieldLeft) && (angle <= fieldRight);
		}
		bool IsFacing(const _Vector& vector, float fieldOfView) const
		{
			return IsFacing(
				vector.X,
				vector.Y,
				vector.Z,
				vector.R,
				fieldOfView
			);
		}

		template<typename _T>
		auto Cast() const
		{
			return _Vector<_T, 4, CAST>(
				CAST<T>::template Cast<_T>(
					_Vector2<T, CAST>::X
				),
				CAST<T>::template Cast<_T>(
					_Vector2<T, CAST>::Y
				),
				CAST<T>::template Cast<_T>(
					_Vector3<T, CAST>::Z
				),
				CAST<T>::template Cast<_T>(R)
			);
		}

		bool operator ==(const _Vector& vector) const
		{
			if (R != vector.R)
				return false;

			return _Vector3<T, CAST>::operator==(vector);
		}
		bool operator !=(const _Vector& vector) const
		{
			return !operator==(vector);
		}
	};

	template<typename T, template<typename> typename CAST>
	using _Vector4 = _Vector<T, 4, CAST>;

	template<typename T, template<typename> typename CAST = Vector_Cast>
	using Vector2 = _Vector2<T, CAST>;

	template<typename T, template<typename> typename CAST = Vector_Cast>
	using Vector3 = _Vector3<T, CAST>;

	template<typename T, template<typename> typename CAST = Vector_Cast>
	using Vector4 = _Vector4<T, CAST>;
}
