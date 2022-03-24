#pragma once
#include "AL/Common.hpp"

#include "Iterator.hpp"
#include "ICollection.hpp"

namespace AL::Collections
{
	template<typename T>
	class ArrayIterator
		: public RandomAccessIterator<T>
	{
		typename RandomAccessIterator<T>::pointer lpValue;

	public:
		typedef typename RandomAccessIterator<T>::pointer           pointer;
		typedef typename RandomAccessIterator<T>::reference         reference;
		typedef typename RandomAccessIterator<T>::value_type        value_type;
		typedef typename RandomAccessIterator<T>::difference_type   difference_type;
		typedef typename RandomAccessIterator<T>::iterator_category iterator_category;

		ArrayIterator()
		{
		}

		ArrayIterator(pointer lpValue)
			: lpValue(
				lpValue
			)
		{
		}

		virtual ~ArrayIterator()
		{
		}

		reference operator * () const
		{
			return *lpValue;
		}
		pointer   operator -> () const
		{
			return lpValue;
		}

		ArrayIterator& operator ++ ()
		{
			++lpValue;

			return *this;
		}
		ArrayIterator& operator -- ()
		{
			--lpValue;

			return *this;
		}

		ArrayIterator operator ++ (int)
		{
			auto it = *this;

			++lpValue;

			return it;
		}
		ArrayIterator operator -- (int)
		{
			auto it = *this;

			--lpValue;

			return it;
		}

		ArrayIterator& operator += (size_t count)
		{
			lpValue += count;

			return *this;
		}
		ArrayIterator& operator -= (size_t count)
		{
			lpValue -= count;

			return *this;
		}

		ArrayIterator operator + (size_t count) const
		{
			auto it = *this;
			it += count;

			return it;
		}
		ArrayIterator operator - (size_t count) const
		{
			auto it = *this;
			it -= count;

			return it;
		}

		Bool operator == (const ArrayIterator& it) const
		{
			if (lpValue != it.lpValue)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const ArrayIterator& it) const
		{
			if (operator==(it))
			{

				return False;
			}

			return True;
		}
	};

	template<typename T>
	class ArrayReverseIterator
		: public RandomAccessIterator<T>
	{
		typename RandomAccessIterator<T>::pointer lpValue;

	public:
		typedef typename RandomAccessIterator<T>::pointer           pointer;
		typedef typename RandomAccessIterator<T>::reference         reference;
		typedef typename RandomAccessIterator<T>::value_type        value_type;
		typedef typename RandomAccessIterator<T>::difference_type   difference_type;
		typedef typename RandomAccessIterator<T>::iterator_category iterator_category;

		ArrayReverseIterator()
		{
		}

		ArrayReverseIterator(pointer lpValue)
			: lpValue(
				lpValue
			)
		{
		}

		virtual ~ArrayReverseIterator()
		{
		}

		reference operator * () const
		{
			return *lpValue;
		}
		pointer   operator -> () const
		{
			return lpValue;
		}

		ArrayReverseIterator& operator ++ ()
		{
			--lpValue;

			return *this;
		}
		ArrayReverseIterator& operator -- ()
		{
			++lpValue;

			return *this;
		}

		ArrayReverseIterator operator ++ (int)
		{
			auto it = *this;

			--lpValue;

			return it;
		}
		ArrayReverseIterator operator -- (int)
		{
			auto it = *this;

			++lpValue;

			return it;
		}

		ArrayReverseIterator& operator += (size_t count)
		{
			lpValue -= count;

			return *this;
		}
		ArrayReverseIterator& operator -= (size_t count)
		{
			lpValue += count;

			return *this;
		}

		ArrayReverseIterator operator + (size_t count) const
		{
			auto it = *this;
			it += count;

			return it;
		}
		ArrayReverseIterator operator - (size_t count) const
		{
			auto it = *this;
			it -= count;

			return it;
		}

		Bool operator == (const ArrayReverseIterator& it) const
		{
			if (lpValue != it.lpValue)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const ArrayReverseIterator& it) const
		{
			if (operator==(it))
			{

				return False;
			}

			return True;
		}
	};

	template<typename T>
	class Array
		: public IBidirectionalCollection<T, ArrayIterator, ArrayReverseIterator>
	{
		T*     lpValues;
		size_t capacity;

	public:
		typedef typename IBidirectionalCollection<T, ArrayIterator, ArrayReverseIterator>::Type                 Type;

		typedef typename IBidirectionalCollection<T, ArrayIterator, ArrayReverseIterator>::Collection           Collection;

		typedef typename IBidirectionalCollection<T, ArrayIterator, ArrayReverseIterator>::Iterator             Iterator;
		typedef typename IBidirectionalCollection<T, ArrayIterator, ArrayReverseIterator>::ConstIterator        ConstIterator;

		typedef typename IBidirectionalCollection<T, ArrayIterator, ArrayReverseIterator>::ReverseIterator      ReverseIterator;
		typedef typename IBidirectionalCollection<T, ArrayIterator, ArrayReverseIterator>::ConstReverseIterator ConstReverseIterator;

		template<size_t S>
		static Void Copy(const Type(&source)[S], Type(&destination)[S])
		{
			Copy(
				&source[0],
				&destination[0],
				S
			);
		}
		template<size_t S_SOURCE, size_t S_DESTINATION>
		static Void Copy(const Type(&source)[S_SOURCE], Type(&destination)[S_DESTINATION])
		{
			Copy(
				&source[0],
				S_SOURCE,
				&destination[0],
				S_DESTINATION
			);
		}
		static Void Copy(const Type* lpSource, Type* lpDestination, size_t count)
		{
			Copy(
				lpSource,
				count,
				lpDestination,
				count
			);
		}
		static Void Copy(const Type* lpSource, size_t sourceCount, Type* lpDestination, size_t destinationCount)
		{
			if constexpr (Is_POD<Type>::Value)
			{
				memcpy(
					lpDestination,
					lpSource,
					((sourceCount <= destinationCount) ? sourceCount : destinationCount) * sizeof(Type)
				);
			}
			else
			{
				for (size_t i = 0; (i < sourceCount) && (i < destinationCount); ++i, ++lpSource, ++lpDestination)
				{
					*lpDestination = *lpSource;
				}
			}
		}

		template<size_t S>
		static Void Move(Type(&source)[S], Type(&destination)[S])
		{
			Move(
				&source[0],
				&destination[0],
				S
			);
		}
		template<size_t S_SOURCE, size_t S_DESTINATION>
		static Void Move(Type(&source)[S_SOURCE], Type(&destination)[S_DESTINATION])
		{
			Move(
				&source[0],
				S_SOURCE,
				&destination[0],
				S_DESTINATION
			);
		}
		static Void Move(Type* lpSource, Type* lpDestination, size_t count)
		{
			Move(
				lpSource,
				count,
				lpDestination,
				count
			);
		}
		static Void Move(Type* lpSource, size_t sourceCount, Type* lpDestination, size_t destinationCount)
		{
			if constexpr (Is_POD<Type>::Value)
			{
				memcpy(
					lpDestination,
					lpSource,
					((sourceCount <= destinationCount) ? sourceCount : destinationCount) * sizeof(Type)
				);
			}
			else
			{
				for (size_t i = 0; (i < sourceCount) && (i < destinationCount); ++i, ++lpSource, ++lpDestination)
				{
					*lpDestination = AL::Move(
						*lpSource
					);
				}
			}
		}

		Array()
			: Array(
				0
			)
		{
		}

		Array(Array&& array)
			: lpValues(
				array.lpValues
			),
			capacity(
				array.capacity
			)
		{
			array.lpValues = nullptr;
			array.capacity = 0;
		}
		Array(const Array& array)
			: lpValues(
				new Type[array.capacity]
			),
			capacity(
				array.capacity
			)
		{
			Assign(
				array.lpValues,
				array.capacity
			);
		}

		explicit Array(size_t capacity)
			: lpValues(
				new Type[capacity]
			),
			capacity(
				capacity
			)
		{
		}

		template<size_t S>
		Array(const Type(&values)[S])
			: Array(
				&values[0],
				S
			)
		{
		}

		Array(const Type& value, size_t count)
			: Array(
				value,
				count,
				0
			)
		{
		}
		Array(const Type& value, size_t count, size_t reserve)
			: lpValues(
				new Type[count + reserve]
			),
			capacity(
				count + reserve
			)
		{
			Fill(
				value,
				count
			);
		}

		Array(const Type* lpValues, size_t count)
			: Array(
				lpValues,
				count,
				0
			)
		{
		}
		Array(const Type* lpValues, size_t count, size_t reserve)
			: lpValues(
				new Type[count + reserve]
			),
			capacity(
				count + reserve
			)
		{
			Assign(
				lpValues,
				count,
				reserve
			);
		}

		virtual ~Array()
		{
			if (lpValues != nullptr)
			{
				delete[] lpValues;
			}
		}

		virtual size_t GetSize() const override
		{
			return capacity;
		}

		virtual size_t GetCapacity() const override
		{
			return capacity;
		}

		// Changes the container size+capacity and retains values
		Void SetSize(size_t value)
		{
			auto capacity = GetCapacity();

			if (auto lpValues = ((capacity != value) ? new Type[value] : nullptr))
			{
				Move(
					this->lpValues,
					capacity,
					lpValues,
					value
				);

				if (this->lpValues != nullptr)
				{

					delete[] this->lpValues;
				}

				this->lpValues = lpValues;
				this->capacity = value;
			}
		}

		// Changes the container size+capacity and discards values
		Void SetCapacity(size_t value)
		{
			if (auto lpValues = ((GetCapacity() != value) ? new Type[value] : nullptr))
			{
				if (this->lpValues != nullptr)
				{

					delete[] this->lpValues;
				}

				this->lpValues = lpValues;
				this->capacity = value;
			}
		}

		Void Fill(const Type& value)
		{
			Fill(
				value,
				GetCapacity()
			);
		}
		Void Fill(const Type& value, size_t count)
		{
			Fill(
				value,
				0,
				count
			);
		}
		Void Fill(const Type& value, size_t index, size_t count)
		{
			auto capacity = GetCapacity();

			if (index >= capacity)
			{

				index = capacity;
			}

			if (count > (capacity - index))
			{

				count = capacity - index;
			}

			if constexpr (Is_Type<Type, uint8>::Value)
			{
				memset(
					&lpValues[index],
					value,
					count
				);
			}
			else if constexpr (Is_Convertable<Type, uint8>::Value)
			{
				memset(
					&lpValues[index],
					static_cast<uint8>(value),
					count
				);
			}
			else
			{
				auto lpValues = &this->lpValues[index];

				for (size_t i = 0; i < count; ++i, ++lpValues)
				{
					*lpValues = value;
				}
			}
		}

		template<size_t S>
		Void Assign(const Type(&values)[S])
		{
			Assign(
				&values[0],
				S
			);
		}
		Void Assign(const Type* lpValues, size_t count)
		{
			SetCapacity(
				count
			);

			Copy(
				lpValues,
				this->lpValues,
				count
			);
		}
		Void Assign(const Type* lpValues, size_t count, size_t reserve)
		{
			SetCapacity(
				count + reserve
			);

			Copy(
				lpValues,
				this->lpValues,
				count
			);
		}

		virtual Iterator      begin() override
		{
			return Iterator(
				&lpValues[0]
			);
		}
		virtual ConstIterator begin() const override
		{
			return ConstIterator(
				&lpValues[0]
			);
		}

		virtual Iterator      end() override
		{
			return Iterator(
				&lpValues[GetCapacity()]
			);
		}
		virtual ConstIterator end() const override
		{
			return ConstIterator(
				&lpValues[GetCapacity()]
			);
		}

		virtual ConstIterator cbegin() const override
		{
			return ConstIterator(
				&lpValues[0]
			);
		}

		virtual ConstIterator cend() const override
		{
			return ConstIterator(
				&lpValues[GetCapacity()]
			);
		}

		virtual ReverseIterator      rbegin() override
		{
			return ReverseIterator(
				&lpValues[GetCapacity() - 1]
			);
		}
		virtual ConstReverseIterator rbegin() const override
		{
			return ConstReverseIterator(
				&lpValues[GetCapacity() - 1]
			);
		}

		virtual ReverseIterator      rend() override
		{
			return ReverseIterator(
				&lpValues[0] - 1
			);
		}
		virtual ConstReverseIterator rend() const override
		{
			return ConstReverseIterator(
				&lpValues[0] - 1
			);
		}

		virtual ConstReverseIterator crbegin() const override
		{
			return ConstReverseIterator(
				&lpValues[GetCapacity() - 1]
			);
		}

		virtual ConstReverseIterator crend() const override
		{
			return ConstReverseIterator(
				&lpValues[0] - 1
			);
		}

		Type&       operator [] (size_t index)
		{
			AL_ASSERT(
				index < GetCapacity(),
				"index out of bounds"
			);

			return lpValues[index];
		}
		const Type& operator [] (size_t index) const
		{
			AL_ASSERT(
				index < GetCapacity(),
				"index out of bounds"
			);

			return lpValues[index];
		}

		Array& operator = (Array&& array)
		{
			if (lpValues != nullptr)
			{

				delete[] lpValues;
			}

			lpValues = array.lpValues;
			array.lpValues = nullptr;

			capacity = array.capacity;
			array.capacity = 0;

			return *this;
		}
		Array& operator = (const Array& array)
		{
			Assign(
				array.lpValues,
				array.capacity
			);

			return *this;
		}

		template<size_t S>
		Array& operator = (const Type(&values)[S])
		{
			Assign(
				values
			);

			return *this;
		}

		Bool operator == (const Array& array) const
		{
			size_t capacity;

			if ((capacity = GetCapacity()) != array.GetCapacity())
			{

				return False;
			}

			if constexpr (Is_POD<Type>::Value)
			{
				if (!memcmp(lpValues, array.lpValues, capacity))
				{

					return False;
				}
			}
			else
			{
				auto lpValues1 = lpValues;
				auto lpValues2 = array.lpValues;

				for (size_t i = 0; i < capacity; ++i, ++lpValues1, ++lpValues2)
				{
					if (*lpValues1 != *lpValues2)
					{

						return False;
					}
				}
			}

			return True;
		}
		Bool operator != (const Array& array) const
		{
			if (operator==(array))
			{

				return False;
			}

			return True;
		}
	};

	template<typename T, size_t S>
	class Array<T[S]>
		: public IBidirectionalCollection<T, ArrayIterator, ArrayReverseIterator>
	{
		T values[S];

	public:
		typedef typename IBidirectionalCollection<T, ArrayIterator, ArrayReverseIterator>::Type                 Type;

		typedef typename IBidirectionalCollection<T, ArrayIterator, ArrayReverseIterator>::Collection           Collection;

		typedef typename IBidirectionalCollection<T, ArrayIterator, ArrayReverseIterator>::Iterator             Iterator;
		typedef typename IBidirectionalCollection<T, ArrayIterator, ArrayReverseIterator>::ConstIterator        ConstIterator;

		typedef typename IBidirectionalCollection<T, ArrayIterator, ArrayReverseIterator>::ReverseIterator      ReverseIterator;
		typedef typename IBidirectionalCollection<T, ArrayIterator, ArrayReverseIterator>::ConstReverseIterator ConstReverseIterator;

		Array()
		{
		}

		Array(Array&& array)
			: values {
				Expand_Move(array.values)
			}
		{
		}
		Array(const Array& array)
			: values {
				Expand_Copy(array.values)
			}
		{
		}

		Array(Type(&&values)[S])
			: values {
				Expand_Move(values)
			}
		{
		}
		Array(const Type(&values)[S])
			: values {
				Expand_Copy(values)
			}
		{
		}

		virtual ~Array()
		{
		}

		virtual size_t GetSize() const override
		{
			return S;
		}

		virtual size_t GetCapacity() const override
		{
			return S;
		}

		Void Fill(const Type& value)
		{
			Fill(
				value,
				GetCapacity()
			);
		}
		Void Fill(const Type& value, size_t count)
		{
			Fill(
				value,
				0,
				count
			);
		}
		Void Fill(const Type& value, size_t index, size_t count)
		{
			auto capacity = GetCapacity();

			if (index >= capacity)
			{

				index = capacity;
			}

			if (count > (capacity - index))
			{

				count = capacity - index;
			}

			if constexpr (Is_Type<Type, uint8>::Value)
			{
				memset(
					&values[index],
					value,
					count
				);
			}
			else if constexpr (Is_Convertable<Type, uint8>::Value)
			{
				memset(
					&values[index],
					static_cast<uint8>(value),
					count
				);
			}
			else
			{
				auto lpValues = &values[index];

				for (size_t i = 0; i < count; ++i, ++lpValues)
				{
					*lpValues = value;
				}
			}
		}

		Void Assign(const Type(&values)[S])
		{
			Array<Type>::Copy(
				&values[0],
				&this->values[0],
				S
			);
		}

		virtual Iterator begin() override
		{
			return Iterator(
				&values[0]
			);
		}
		virtual ConstIterator begin() const override
		{
			return ConstIterator(
				&values[0]
			);
		}

		virtual Iterator end() override
		{
			return Iterator(
				&values[S]
			);
		}
		virtual ConstIterator end() const override
		{
			return ConstIterator(
				&values[S]
			);
		}

		virtual ConstIterator cbegin() const override
		{
			return ConstIterator(
				&values[0]
			);
		}

		virtual ConstIterator cend() const override
		{
			return ConstIterator(
				&values[S]
			);
		}

		virtual ReverseIterator rbegin() override
		{
			return ReverseIterator(
				&values[S - 1]
			);
		}
		virtual ConstReverseIterator rbegin() const override
		{
			return ConstReverseIterator(
				&values[S - 1]
			);
		}

		virtual ReverseIterator rend() override
		{
			return ReverseIterator(
				&values[0] - 1
			);
		}
		virtual ConstReverseIterator rend() const override
		{
			return ConstReverseIterator(
				&values[0] - 1
			);
		}

		virtual ConstReverseIterator crbegin() const override
		{
			return ConstReverseIterator(
				&values[GetCapacity() - 1]
			);
		}

		virtual ConstReverseIterator crend() const override
		{
			return ConstReverseIterator(
				&values[0] - 1
			);
		}

		Type& operator [] (size_t index)
		{
			AL_ASSERT(
				index < GetCapacity(),
				"index out of bounds"
			);

			return values[index];
		}
		const Type& operator [] (size_t index) const
		{
			AL_ASSERT(
				index < GetCapacity(),
				"index out of bounds"
			);

			return values[index];
		}

		Array& operator = (Array&& array)
		{
			Array<Type>::Move(
				array.values,
				values
			);

			return *this;
		}
		Array& operator = (const Array& array)
		{
			Array<Type>::Copy(
				array.values,
				values
			);

			return *this;
		}

		Array& operator = (const Type(&values)[S])
		{
			Assign(
				values
			);

			return *this;
		}

		Bool operator == (const Array& array) const
		{
			if constexpr (Is_POD<Type>::Value)
			{
				if (!memcmp(values, array.values))
				{

					return False;
				}
			}
			else
			{
				auto lpValues1 = &values[0];
				auto lpValues2 = &array.values[0];

				for (size_t i = 0; i < S; ++i, ++lpValues1, ++lpValues2)
				{
					if (*lpValues1 != *lpValues2)
					{

						return False;
					}
				}
			}

			return True;
		}
		Bool operator != (const Array& array) const
		{
			if (operator==(array))
			{

				return False;
			}

			return True;
		}
	};
}

template<typename T>
struct AL::Is_Array<AL::Collections::Array<T>>
{
	static constexpr Bool Value = True;
};
template<typename T, AL::size_t S>
struct AL::Is_Array<AL::Collections::Array<T[S]>>
{
	static constexpr Bool Value = True;
};

template<typename T>
struct AL::Get_Array_Type<AL::Collections::Array<T>>
{
	typedef T Type;
};
template<typename T, AL::size_t S>
struct AL::Get_Array_Type<AL::Collections::Array<T[S]>>
{
	typedef T Type;
};
