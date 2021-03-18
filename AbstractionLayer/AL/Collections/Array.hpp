#pragma once
#include "AL/Common.hpp"

#include "Iterator.hpp"

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

		explicit ArrayIterator(reference value)
			: lpValue(
				&value
			)
		{
		}

		reference operator * () const
		{
			return *lpValue;
		}
		reference operator -> () const
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

		ArrayIterator& operator += (size_t offset)
		{
			lpValue += offset;

			return *this;
		}
		ArrayIterator& operator -= (size_t offset)
		{
			lpValue -= offset;

			return *this;
		}

		bool operator > (const ArrayIterator& it) const
		{
			return lpValue > it.lpValue;
		}
		bool operator < (const ArrayIterator& it) const
		{
			return lpValue < it.lpValue;
		}

		bool operator == (const ArrayIterator& it) const
		{
			return lpValue == it.lpValue;
		}
		bool operator != (const ArrayIterator& it) const
		{
			return !operator==(it);
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

		ArrayReverseIterator(reference value)
			: lpValue(
				&value
			)
		{
		}

		reference operator * () const
		{
			return *(lpValue - 1);
		}
		reference operator -> () const
		{
			return (lpValue - 1);
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

		ArrayReverseIterator& operator += (size_t offset)
		{
			lpValue -= offset;

			return *this;
		}
		ArrayReverseIterator& operator -= (size_t offset)
		{
			lpValue += offset;

			return *this;
		}

		bool operator > (const ArrayReverseIterator& it) const
		{
			return lpValue < it.lpValue;
		}
		bool operator < (const ArrayReverseIterator& it) const
		{
			return lpValue > it.lpValue;
		}

		bool operator == (const ArrayReverseIterator& it) const
		{
			return lpValue == it.lpValue;
		}
		bool operator != (const ArrayReverseIterator& it) const
		{
			return !operator==(it);
		}
	};

	template<typename T>
	class Array
	{
		size_t size;
		size_t capacity;

		T* lpContainer;

	public:
		typedef T                             Type;

		typedef ArrayIterator<T>              Iterator;
		typedef ArrayIterator<const T>        ConstIterator;

		typedef ArrayReverseIterator<T>       ReverseIterator;
		typedef ArrayReverseIterator<const T> ConstReverseIterator;

		Array()
			: size(
				0
			),
			capacity(
				0
			),
			lpContainer(
				nullptr
			)
		{
		}

		Array(Array&& array)
			: size(
				array.GetSize()
			),
			capacity(
				array.GetCapacity()
			),
			lpContainer(
				array.lpContainer
			)
		{
			array.size = 0;
			array.capacity = 0;
			array.lpContainer = nullptr;
		}

		Array(const Array& array)
			: size(
				array.GetSize()
			),
			capacity(
				array.GetCapacity()
			),
			lpContainer(
				new T[array.GetCapacity()]
			)
		{
			for (size_t i = 0; i < GetSize(); ++i)
			{
				lpContainer[i] = array.lpContainer[i];
			}
		}

		explicit Array(size_t capacity)
			: size(
				0
			),
			capacity(
				capacity
			),
			lpContainer(
				new T[capacity]
			)
		{
		}

		template<size_t SIZE>
		Array(Type(&&values)[SIZE])
			: size(
				SIZE
			),
			capacity(
				SIZE
			),
			lpContainer(
				new T[SIZE]
			)
		{
			for (size_t i = 0; i < SIZE; ++i)
			{
				lpContainer[i] = Move(
					values[i]
				);
			}
		}

		template<size_t SIZE>
		Array(const Type(&values)[SIZE])
			: size(
				SIZE
			),
			capacity(
				SIZE
			),
			lpContainer(
				new T[SIZE]
			)
		{
			for (size_t i = 0; i < SIZE; ++i)
			{
				lpContainer[i] = values[i];
			}
		}

		template<typename ITERATOR>
		Array(ITERATOR first, ITERATOR last)
			: size(
				GetIteratorDifference(
					last,
					first
				)
			),
			capacity(
				size
			),
			lpContainer(
				new T[size]
			)
		{
			for (size_t i = 0; i < GetSize(); ++i, ++first)
			{
				lpContainer[i] = *first;
			}
		}

		Array(const Type* lpValues, size_t count)
			: size(
				count
			),
			capacity(
				count
			),
			lpContainer(
				new T[count]
			)
		{
			for (size_t i = 0; i < GetSize(); ++i)
			{
				lpContainer[i] = lpValues[i];
			}
		}

		virtual ~Array()
		{
			if (lpContainer != nullptr)
			{
				delete[] lpContainer;
			}
		}

		size_t GetSize() const
		{
			return size;
		}

		size_t GetCapacity() const
		{
			return capacity;
		}

		void Clear()
		{
			if (lpContainer != nullptr)
			{

				delete[] lpContainer;
			}

			size = 0;
			capacity = 0;
			lpContainer = nullptr;
		}

		void ShrinkToFit()
		{
			if (GetSize() < GetCapacity())
			{
				auto lpContainer = new T[GetSize()];

				for (size_t i = 0; i < GetSize(); ++i)
				{
					lpContainer[i] = Move(
						this->lpContainer[i]
					);
				}

				capacity = GetSize();

				delete[] this->lpContainer;
				this->lpContainer = lpContainer;
			}
		}

		void Swap(Array& array)
		{
			AL::Swap(
				size,
				array.size
			);

			AL::Swap(
				capacity,
				array.capacity
			);

			AL::Swap(
				lpContainer,
				array.lpContainer
			);
		}

		void Fill(const Type& value)
		{
			for (size_t i = 0; i < GetCapacity(); ++i)
			{
				lpContainer[i] = value;
			}
		}

		void Reserve(size_t value)
		{
			SetCapacity(
				GetCapacity() + value
			);
		}

		void Erase(size_t index)
		{
			Erase(
				index,
				1
			);
		}
		void Erase(size_t index, size_t count)
		{
			for (size_t i = (index + count), size = GetSize(); i < size; ++i)
			{
				--this->size;

				lpContainer[i - count] = Move(
					lpContainer[i]
				);
			}
		}

		void Erase(Iterator it)
		{
			auto index = GetIteratorDifference(
				begin(),
				it
			);

			Erase(
				index
			);
		}
		void Erase(ConstIterator it)
		{
			auto index = GetIteratorDifference(
				cbegin(),
				it
			);

			Erase(
				index
			);
		}
		void Erase(ReverseIterator it)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				it
			);

			Erase(
				index
			);
		}
		void Erase(ConstReverseIterator it)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				it
			);

			Erase(
				index
			);
		}

		void Erase(Iterator first, Iterator last)
		{
			auto index = GetIteratorDifference(
				begin(),
				first
			);

			auto count = GetIteratorDifference(
				first,
				last
			);

			Erase(
				index,
				count
			);
		}
		void Erase(ConstIterator first, ConstIterator last)
		{
			auto index = GetIteratorDifference(
				cbegin(),
				first
			);

			auto count = GetIteratorDifference(
				first,
				last
			);

			Erase(
				index,
				count
			);
		}
		void Erase(ReverseIterator first, ReverseIterator last)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				first
			);

			auto count = GetIteratorDifference(
				first,
				last
			);

			Erase(
				index,
				count
			);
		}
		void Erase(ConstReverseIterator first, ConstReverseIterator last)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				first
			);

			auto count = GetIteratorDifference(
				first,
				last
			);

			Erase(
				index,
				count
			);
		}

		void PopBack()
		{
			if (GetSize() > 0)
			{
				--size;
			}
		}

		void PushBack(Type&& value)
		{
			if (GetSize() == GetCapacity())
			{
				Reserve(
					1
				);
			}

			lpContainer[size++] = Move(
				value
			);
		}
		void PushBack(const Type& value)
		{
			PushBack(
				Type(value)
			);
		}

		template<typename ... TArgs>
		void EmplaceBack(TArgs ... args)
		{
			PushBack(
				Type(Forward<TArgs>(args) ...)
			);
		}

		void Insert(size_t index, Type&& value)
		{
			if (GetSize() == GetCapacity())
			{
				Reserve(
					1
				);
			}

			for (size_t i = GetSize(); i-- > index; )
			{
				lpContainer[i + 1] = Move(
					lpContainer[i]
				);
			}

			lpContainer[index] = Move(
				value
			);

			++size;
		}
		void Insert(size_t index, const Type& value)
		{
			Insert(
				index,
				Type(value)
			);
		}
		void Insert(size_t index, const Type* lpValues, size_t count)
		{
			auto newSize = GetSize() + count;

			if (auto missingElementCount = ((newSize > GetCapacity()) ? newSize - GetCapacity() : 0))
			{
				Reserve(
					missingElementCount
				);
			}

			for (size_t i = GetSize(); i-- > index; )
			{
				lpContainer[i + count] = Move(
					lpContainer[i]
				);
			}

			for (size_t i = index, j = 0; i < count; ++i, ++j)
			{
				lpContainer[i] = lpValues[j];
			}

			size = newSize;
		}

		void Insert(Iterator it, Type&& value)
		{
			auto index = GetIteratorDifference(
				begin(),
				it
			);

			Insert(
				index,
				Move(value)
			);
		}
		void Insert(ConstIterator it, Type&& value)
		{
			auto index = GetIteratorDifference(
				cbegin(),
				it
			);

			Insert(
				index,
				Move(value)
			);
		}
		void Insert(ReverseIterator it, Type&& value)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				it
			);

			Insert(
				index,
				Move(value)
			);
		}
		void Insert(ConstReverseIterator it, Type&& value)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				it
			);

			Insert(
				index,
				Move(value)
			);
		}

		void Insert(Iterator it, const Type& value)
		{
			Insert(
				it,
				Type(value)
			);
		}
		void Insert(ConstIterator it, const Type& value)
		{
			Insert(
				it,
				Type(value)
			);
		}
		void Insert(ReverseIterator it, const Type& value)
		{
			Insert(
				it,
				Type(value)
			);
		}
		void Insert(ConstReverseIterator it, const Type& value)
		{
			Insert(
				it,
				Type(value)
			);
		}

		template<typename ITERATOR>
		void Insert(Iterator it, ITERATOR first, ITERATOR last)
		{
			auto index = GetIteratorDifference(
				begin(),
				it
			);

			auto count = GetIteratorDifference(
				first,
				last
			);

			Insert(
				index,
				&(*first),
				count
			);
		}
		template<typename ITERATOR>
		void Insert(ConstIterator it, ITERATOR first, ITERATOR last)
		{
			auto index = GetIteratorDifference(
				cbegin(),
				it
			);

			auto count = GetIteratorDifference(
				first,
				last
			);

			Insert(
				index,
				&(*first),
				count
			);
		}
		template<typename ITERATOR>
		void Insert(ReverseIterator it, ITERATOR first, ITERATOR last)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				it
			);

			auto count = GetIteratorDifference(
				first,
				last
			);

			Insert(
				index,
				&(*first),
				count
			);
		}
		template<typename ITERATOR>
		void Insert(ConstReverseIterator it, ITERATOR first, ITERATOR last)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				it
			);

			auto count = GetIteratorDifference(
				first,
				last
			);

			Insert(
				index,
				&(*first),
				count
			);
		}

		void SetSize(size_t value)
		{
			if (value > GetCapacity())
			{
				auto lpContainer = new T[value];

				for (size_t i = 0; i < GetSize(); ++i)
				{
					lpContainer[i] = Move(
						this->lpContainer[i]
					);
				}

				capacity = value;

				if (this->lpContainer != nullptr)
				{

					delete[] this->lpContainer;
				}

				this->lpContainer = lpContainer;
			}

			size = value;
		}

		void SetCapacity(size_t value)
		{
			if (value > GetCapacity())
			{
				auto lpContainer = new T[value];

				for (size_t i = 0; i < GetSize(); ++i)
				{
					lpContainer[i] = Move(
						this->lpContainer[i]
					);
				}

				capacity = value;

				if (this->lpContainer != nullptr)
				{

					delete[] this->lpContainer;
				}

				this->lpContainer = lpContainer;
			}
			else if (value < GetCapacity())
			{
				auto lpContainer = new T[value];

				if (value < GetSize())
				{

					size = value;
				}

				for (size_t i = 0; i < GetSize(); ++i)
				{
					lpContainer[i] = Move(
						this->lpContainer[i]
					);
				}

				capacity = value;

				delete[] this->lpContainer;
				this->lpContainer = lpContainer;
			}
		}

		Iterator begin()
		{
			return Iterator(
				lpContainer[0]
			);
		}
		ConstIterator begin() const
		{
			return ConstIterator(
				lpContainer[0]
			);
		}

		Iterator end()
		{
			return Iterator(
				lpContainer[GetSize()]
			);
		}
		ConstIterator end() const
		{
			return ConstIterator(
				lpContainer[GetSize()]
			);
		}

		ConstIterator cbegin() const
		{
			return ConstIterator(
				lpContainer[0]
			);
		}

		ConstIterator cend() const
		{
			return ConstIterator(
				lpContainer[GetSize()]
			);
		}

		ReverseIterator rbegin()
		{
			return ReverseIterator(
				lpContainer[GetSize()]
			);
		}
		ConstReverseIterator rbegin() const
		{
			return ConstReverseIterator(
				lpContainer[GetSize()]
			);
		}

		ReverseIterator rend()
		{
			return ReverseIterator(
				lpContainer[0]
			);
		}
		ConstReverseIterator rend() const
		{
			return ConstReverseIterator(
				lpContainer[0]
			);
		}

		auto& operator [] (size_t index)
		{
			return lpContainer[index];
		}
		auto& operator [] (size_t index) const
		{
			return lpContainer[index];
		}

		auto& operator = (Array&& array)
		{
			if (lpContainer != nullptr)
			{

				delete[] lpContainer;
			}

			size = array.size;
			capacity = array.capacity;
			lpContainer = array.lpContainer;

			array.size = 0;
			array.capacity = 0;
			array.lpContainer = nullptr;

			return *this;
		}
		auto& operator = (const Array& array)
		{
			if (lpContainer == nullptr)
			{
				size = array.GetSize();
				capacity = array.GetCapacity();
				lpContainer = new T[array.GetCapacity()];

				for (size_t i = 0; i < array.GetSize(); ++i)
				{
					lpContainer[i] = array[i];
				}
			}
			else if (GetCapacity() == array.GetCapacity())
			{
				size = array.GetSize();

				for (size_t i = 0; i < GetSize(); ++i)
				{
					lpContainer[i] = array[i];
				}
			}
			else
			{
				delete[] lpContainer;

				size = array.GetSize();
				capacity = array.GetCapacity();
				lpContainer = new T[array.GetCapacity()];

				for (size_t i = 0; i < array.GetSize(); ++i)
				{
					lpContainer[i] = array[i];
				}
			}

			return *this;
		}

		bool operator == (const Array& array) const
		{
			if (GetSize() != array.GetSize())
			{

				return false;
			}

			if constexpr (Is_POD<T>::Value)
			{
				if (!memcmp(&lpContainer[0], &array[0], GetSize()))
				{

					return false;
				}
			}
			else
			{
				for (size_t i = 0; i < GetSize(); ++i)
				{
					if (lpContainer[i] != array[i])
					{

						return false;
					}
				}
			}

			return true;
		}
		bool operator != (const Array& array) const
		{
			return !operator==(array);
		}
	};
	
	template<typename T, size_t SIZE>
	class Array<T[SIZE]>
	{
		size_t size;

		T container[SIZE];

	public:
		typedef T                             Type;

		typedef ArrayIterator<T>              Iterator;
		typedef ArrayIterator<const T>        ConstIterator;

		typedef ArrayReverseIterator<T>       ReverseIterator;
		typedef ArrayReverseIterator<const T> ConstReverseIterator;

		Array()
			: size(
				0
			)
		{
		}

		Array(Array&& array)
			: size(
				array.GetSize()
			)
		{
			for (size_t i = 0; i < array.GetSize(); ++i)
			{
				container[i] = Move(
					array[i]
				);
			}
		}

		Array(const Array& array)
			: size(
				array.GetSize()
			)
		{
			for (size_t i = 0; i < array.GetSize(); ++i)
			{
				container[i] = array[i];
			}
		}

		Array(Type(&&values)[SIZE])
			: size(
				SIZE
			)
		{
			for (size_t i = 0; i < SIZE; ++i)
			{
				container[i] = Move(
					values[i]
				);
			}
		}

		Array(const Type(&values)[SIZE])
			: size(
				SIZE
			)
		{
			for (size_t i = 0; i < SIZE; ++i)
			{
				container[i] = values[i];
			}
		}
		
		virtual ~Array()
		{
		}

		size_t GetSize() const
		{
			return size;
		}

		static constexpr size_t GetCapacity()
		{
			return SIZE;
		}

		void Clear()
		{
			size = 0;
		}

		void Swap(Array& array)
		{
			AL::Swap(
				size,
				array.size
			);

			AL::Swap(
				container,
				array.container
			);
		}

		void Fill(const Type& value)
		{
			for (size_t i = 0; i < GetCapacity(); ++i)
			{
				container[i] = value;
			}

			size = GetCapacity();
		}

		void Erase(size_t index)
		{
			Erase(
				index,
				1
			);
		}
		void Erase(size_t index, size_t count)
		{
			for (size_t i = (index + count), size = GetSize(); i < size; ++i)
			{
				--this->size;

				container[i - count] = Move(
					container[i]
				);
			}
		}

		void Erase(Iterator it)
		{
			auto index = GetIteratorDifference(
				begin(),
				it
			);

			Erase(
				index
			);
		}
		void Erase(ConstIterator it)
		{
			auto index = GetIteratorDifference(
				cbegin(),
				it
			);

			Erase(
				index
			);
		}
		void Erase(ReverseIterator it)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				it
			);

			Erase(
				index
			);
		}
		void Erase(ConstReverseIterator it)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				it
			);

			Erase(
				index
			);
		}

		void Erase(Iterator first, Iterator last)
		{
			auto index = GetIteratorDifference(
				begin(),
				first
			);

			auto count = GetIteratorDifference(
				first,
				last
			);

			Erase(
				index,
				count
			);
		}
		void Erase(ConstIterator first, ConstIterator last)
		{
			auto index = GetIteratorDifference(
				cbegin(),
				first
			);

			auto count = GetIteratorDifference(
				first,
				last
			);

			Erase(
				index,
				count
			);
		}
		void Erase(ReverseIterator first, ReverseIterator last)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				first
			);

			auto count = GetIteratorDifference(
				first,
				last
			);

			Erase(
				index,
				count
			);
		}
		void Erase(ConstReverseIterator first, ConstReverseIterator last)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				first
			);

			auto count = GetIteratorDifference(
				first,
				last
			);

			Erase(
				index,
				count
			);
		}

		void PopBack()
		{
			if (GetSize() > 0)
			{
				--size;
			}
		}

		// @return false if full
		bool PushBack(Type&& value)
		{
			if (GetSize() == GetCapacity())
			{

				return false;
			}

			container[size++] = Move(
				value
			);

			return true;
		}
		// @return false if full
		bool PushBack(const Type& value)
		{
			return PushBack(
				Type(value)
			);
		}

		// @return false if full
		template<typename ... TArgs>
		bool EmplaceBack(TArgs ... args)
		{
			return PushBack(
				Type(Forward<TArgs>(args) ...)
			);
		}

		// @return false if full
		bool Insert(size_t index, Type&& value)
		{
			if (GetSize() == GetCapacity())
			{

				return false;
			}

			for (size_t i = GetSize(); i-- > index; )
			{
				container[i + 1] = Move(
					container[i]
				);
			}

			container[index] = Move(
				value
			);

			++size;

			return true;
		}
		// @return false if full
		bool Insert(size_t index, const Type& value)
		{
			return Insert(
				index,
				Type(value)
			);
		}
		// @return false if full
		bool Insert(size_t index, const Type* lpValues, size_t count)
		{
			auto newSize = GetSize() + count;

			if (newSize > GetCapacity())
			{

				return false;
			}

			for (size_t i = GetSize(); i-- > index; )
			{
				container[i + count] = Move(
					container[i]
				);
			}

			for (size_t i = index, j = 0; i < count; ++i, ++j)
			{
				container[i] = lpValues[j];
			}

			size = newSize;

			return true;
		}

		// @return false if full
		bool Insert(Iterator it, Type&& value)
		{
			auto index = GetIteratorDifference(
				begin(),
				it
			);

			return Insert(
				index,
				Move(value)
			);
		}
		// @return false if full
		bool Insert(ConstIterator it, Type&& value)
		{
			auto index = GetIteratorDifference(
				cbegin(),
				it
			);

			return Insert(
				index,
				Move(value)
			);
		}
		// @return false if full
		bool Insert(ReverseIterator it, Type&& value)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				it
			);

			return Insert(
				index,
				Move(value)
			);
		}
		// @return false if full
		bool Insert(ConstReverseIterator it, Type&& value)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				it
			);

			return Insert(
				index,
				Move(value)
			);
		}

		// @return false if full
		bool Insert(Iterator it, const Type& value)
		{
			return Insert(
				it,
				Type(value)
			);
		}
		// @return false if full
		bool Insert(ConstIterator it, const Type& value)
		{
			return Insert(
				it,
				Type(value)
			);
		}
		// @return false if full
		bool Insert(ReverseIterator it, const Type& value)
		{
			return Insert(
				it,
				Type(value)
			);
		}
		// @return false if full
		bool Insert(ConstReverseIterator it, const Type& value)
		{
			return Insert(
				it,
				Type(value)
			);
		}

		// @return false if full
		template<typename ITERATOR>
		bool Insert(Iterator it, ITERATOR first, ITERATOR last)
		{
			auto index = GetIteratorDifference(
				begin(),
				it
			);

			auto count = GetIteratorDifference(
				first,
				last
			);

			return Insert(
				index,
				&(*first),
				count
			);
		}
		// @return false if full
		template<typename ITERATOR>
		bool Insert(ConstIterator it, ITERATOR first, ITERATOR last)
		{
			auto index = GetIteratorDifference(
				cbegin(),
				it
			);

			auto count = GetIteratorDifference(
				first,
				last
			);

			return Insert(
				index,
				&(*first),
				count
			);
		}
		// @return false if full
		template<typename ITERATOR>
		bool Insert(ReverseIterator it, ITERATOR first, ITERATOR last)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				it
			);

			auto count = GetIteratorDifference(
				first,
				last
			);

			return Insert(
				index,
				&(*first),
				count
			);
		}
		// @return false if full
		template<typename ITERATOR>
		bool Insert(ConstReverseIterator it, ITERATOR first, ITERATOR last)
		{
			auto index = GetIteratorDifference(
				rbegin(),
				it
			);

			auto count = GetIteratorDifference(
				first,
				last
			);

			return Insert(
				index,
				&(*first),
				count
			);
		}

		Iterator begin()
		{
			return Iterator(
				container[0]
			);
		}
		ConstIterator begin() const
		{
			return ConstIterator(
				container[0]
			);
		}

		Iterator end()
		{
			return Iterator(
				container[GetSize()]
			);
		}
		ConstIterator end() const
		{
			return ConstIterator(
				container[GetSize()]
			);
		}

		ConstIterator cbegin() const
		{
			return ConstIterator(
				container[0]
			);
		}

		ConstIterator cend() const
		{
			return ConstIterator(
				container[GetSize()]
			);
		}

		ReverseIterator rbegin()
		{
			return ReverseIterator(
				container[GetSize()]
			);
		}
		ConstReverseIterator rbegin() const
		{
			return ConstReverseIterator(
				container[GetSize()]
			);
		}

		ReverseIterator rend()
		{
			return ReverseIterator(
				container[0]
			);
		}
		ConstReverseIterator rend() const
		{
			return ConstReverseIterator(
				container[0]
			);
		}

		auto& operator [] (size_t index)
		{
			return container[index];
		}
		auto& operator [] (size_t index) const
		{
			return container[index];
		}

		auto& operator = (Array&& array)
		{
			size = array.size;
			array.size = 0;

			for (size_t i = 0; i < array.GetSize(); ++i)
			{
				container[i] = Move(
					array[i]
				);
			}

			return *this;
		}
		auto& operator = (const Array& array)
		{
			size = array.size;

			for (size_t i = 0; i < array.GetSize(); ++i)
			{
				container[i] = array[i];
			}

			return *this;
		}

		bool operator == (const Array& array) const
		{
			if constexpr (Is_POD<T>::Value)
			{
				if (!memcmp(&container[0], &array[0], GetSize()))
				{

					return false;
				}
			}
			else
			{
				for (size_t i = 0; i < GetSize(); ++i)
				{
					if (container[i] != array[i])
					{

						return false;
					}
				}
			}

			return true;
		}
		bool operator != (const Array& array) const
		{
			return !operator==(array);
		}
	};
}
