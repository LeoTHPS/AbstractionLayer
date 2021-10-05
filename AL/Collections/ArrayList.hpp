#pragma once
#include "AL/Common.hpp"

#include "Array.hpp"
#include "Iterator.hpp"
#include "ICollection.hpp"

namespace AL::Collections
{
	template<typename T>
	class ArrayListIterator
		: public RandomAccessIterator<T>
	{
		typename RandomAccessIterator<T>::pointer lpValue;

	public:
		typedef typename RandomAccessIterator<T>::pointer           pointer;
		typedef typename RandomAccessIterator<T>::reference         reference;
		typedef typename RandomAccessIterator<T>::value_type        value_type;
		typedef typename RandomAccessIterator<T>::difference_type   difference_type;
		typedef typename RandomAccessIterator<T>::iterator_category iterator_category;

		ArrayListIterator()
		{
		}

		ArrayListIterator(pointer lpValue)
			: lpValue(
				lpValue
			)
		{
		}

		virtual ~ArrayListIterator()
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

		ArrayListIterator& operator ++ ()
		{
			++lpValue;

			return *this;
		}
		ArrayListIterator& operator -- ()
		{
			--lpValue;

			return *this;
		}

		ArrayListIterator operator ++ (int)
		{
			auto it = *this;

			++lpValue;

			return it;
		}
		ArrayListIterator operator -- (int)
		{
			auto it = *this;

			--lpValue;

			return it;
		}

		ArrayListIterator& operator += (size_t count)
		{
			lpValue += count;

			return *this;
		}
		ArrayListIterator& operator -= (size_t count)
		{
			lpValue -= count;

			return *this;
		}

		Bool operator == (const ArrayListIterator& it) const
		{
			if (lpValue != it.lpValue)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const ArrayListIterator& it) const
		{
			if (operator==(it))
			{

				return False;
			}

			return True;
		}
	};

	template<typename T>
	class ArrayListReverseIterator
		: public RandomAccessIterator<T>
	{
		typename RandomAccessIterator<T>::pointer lpValue;

	public:
		typedef typename RandomAccessIterator<T>::pointer           pointer;
		typedef typename RandomAccessIterator<T>::reference         reference;
		typedef typename RandomAccessIterator<T>::value_type        value_type;
		typedef typename RandomAccessIterator<T>::difference_type   difference_type;
		typedef typename RandomAccessIterator<T>::iterator_category iterator_category;

		ArrayListReverseIterator()
		{
		}

		ArrayListReverseIterator(pointer lpValue)
			: lpValue(
				lpValue
			)
		{
		}

		virtual ~ArrayListReverseIterator()
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

		ArrayListReverseIterator& operator ++ ()
		{
			--lpValue;

			return *this;
		}
		ArrayListReverseIterator& operator -- ()
		{
			++lpValue;

			return *this;
		}

		ArrayListReverseIterator operator ++ (int)
		{
			auto it = *this;

			--lpValue;

			return it;
		}
		ArrayListReverseIterator operator -- (int)
		{
			auto it = *this;

			++lpValue;

			return it;
		}
		
		ArrayListReverseIterator& operator += (size_t count)
		{
			lpValue += count;

			return *this;
		}
		ArrayListReverseIterator& operator -= (size_t count)
		{
			lpValue -= count;

			return *this;
		}

		Bool operator == (const ArrayListReverseIterator& it) const
		{
			if (lpValue != it.lpValue)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const ArrayListReverseIterator& it) const
		{
			if (operator==(it))
			{

				return False;
			}

			return True;
		}
	};

	template<typename T>
	class ArrayList
		: public Array<T>::Collection
	{
		size_t   size = 0;
		Array<T> container;

	public:
		typedef T                                         Type;

		typedef typename Array<Type>::Collection          Collection;

		typedef typename Collection::Iterator             Iterator;
		typedef typename Collection::ConstIterator        ConstIterator;

		typedef typename Collection::ReverseIterator      ReverseIterator;
		typedef typename Collection::ConstReverseIterator ConstReverseIterator;

		ArrayList()
			: ArrayList(
				0
			)
		{
		}

		ArrayList(ArrayList&& arrayList)
			: size(
				arrayList.size
			),
			container(
				Move(arrayList.container)
			)
		{
			arrayList.size = 0;
		}
		ArrayList(const ArrayList& arrayList)
			: ArrayList(
				&arrayList.container[0],
				arrayList.size,
				arrayList.GetCapacity() - arrayList.size
			)
		{
		}

		explicit ArrayList(size_t capacity)
			: ArrayList(
				Type(),
				0,
				capacity
			)
		{
		}

		template<size_t S>
		ArrayList(const Type(&container)[S])
			: ArrayList(
				&container[0],
				S,
				0
			)
		{
		}

		ArrayList(const Type& value, size_t count)
			: ArrayList(
				value,
				count,
				0
			)
		{
		}
		ArrayList(const Type& value, size_t count, size_t reserve)
			: size(
				count
			),
			container(
				value,
				count,
				reserve
			)
		{
		}

		ArrayList(const Type* lpValues, size_t count)
			: ArrayList(
				lpValues,
				count,
				0
			)
		{
		}
		ArrayList(const Type* lpValues, size_t count, size_t reserve)
			: size(
				count
			),
			container(
				lpValues,
				count,
				reserve
			)
		{
		}

		virtual ~ArrayList()
		{
		}

		virtual size_t GetSize() const override
		{
			return size;
		}

		virtual size_t GetCapacity() const override
		{
			return container.GetCapacity();
		}

		Iterator      Find(const Type& value)
		{
			for (auto it = begin(); it != end(); ++it)
			{
				if (*it == value)
				{

					return it;
				}
			}

			return end();
		}
		ConstIterator Find(const Type& value) const
		{
			for (auto it = cbegin(); it != cend(); ++it)
			{
				if (*it == value)
				{

					return it;
				}
			}

			return cend();
		}

		ReverseIterator      FindLast(const Type& value)
		{
			for (auto it = rbegin(); it != rend(); ++it)
			{
				if (*it == value)
				{

					return it;
				}
			}

			return rend();
		}
		ConstReverseIterator FindLast(const Type& value) const
		{
			for (auto it = crbegin(); it != crend(); ++it)
			{
				if (*it == value)
				{

					return it;
				}
			}

			return crend();
		}

		Bool Contains(const Type& value) const
		{
			for (auto& _value : container)
			{
				if (_value == value)
				{

					return True;
				}
			}

			return False;
		}

		Void SetCapacity(size_t value)
		{
			container.SetSize(
				value
			);

			if (GetSize() > value)
			{

				size = value;
			}
		}

		Void Reserve(size_t count)
		{
			SetCapacity(
				GetCapacity() + count
			);
		}

		Void Clear()
		{
			size = 0;
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

			if (index > capacity)
			{

				index = capacity;
			}

			if (count > (capacity - index))
			{

				count = capacity - index;
			}

			container.Fill(
				value,
				index,
				count
			);

			size = count;
		}

		Void PopBack()
		{
			Erase(
				--end()
			);
		}

		Void PopFront()
		{
			Erase(
				begin()
			);
		}

		Void PushBack(Type&& value)
		{
			Insert(
				end(),
				Move(value)
			);
		}
		Void PushBack(const Type& value)
		{
			Insert(
				end(),
				value
			);
		}

		Void PushFront(Type&& value)
		{
			Insert(
				begin(),
				Move(value)
			);
		}
		Void PushFront(const Type& value)
		{
			Insert(
				begin(),
				value
			);
		}

		template<typename T_ITERATOR, typename ... TArgs>
		Void Emplace(T_ITERATOR it, TArgs ... args)
		{
			Type value(
				Forward<TArgs>(args) ...
			);

			Insert(
				it,
				Move(value)
			);
		}

		template<typename ... TArgs>
		Void EmplaceBack(TArgs ... args)
		{
			Emplace(
				end(),
				Forward<TArgs>(args) ...
			);
		}

		template<typename ... TArgs>
		Void EmplaceFront(TArgs ... args)
		{
			Emplace(
				begin(),
				Forward<TArgs>(args) ...
			);
		}

		template<typename T_ITERATOR>
		Void Erase(T_ITERATOR it)
		{
			Erase(
				it,
				it
			);
		}
		template<typename T_ITERATOR>
		Void Erase(T_ITERATOR first, T_ITERATOR last)
		{
			size_t i_first = &(*first) - &container[0];
			size_t i_last  = &(*last) - &container[0];
			size_t i_count = (i_last - i_first) + 1;

			if (i_last < (GetSize() - 1))
			{
				Array<Type>::Move(
					&container[i_last + 1],
					&container[i_first],
					i_count
				);
			}

			size -= i_count;
		}

		Void Remove(const Type& value)
		{
			auto it = Find(
				value
			);

			if (it != end())
			{

				Erase(
					it
				);
			}
		}

		template<typename T_ITERATOR>
		T_ITERATOR Insert(T_ITERATOR it, Type&& value)
		{
			auto size = GetSize();

			size_t i = &(*it) - &container[0];

			if (size == GetCapacity())
			{

				Reserve(
					1
				);
			}

			for (size_t j = size; j > i; --j)
			{
				Array<Type>::Move(
					&container[j - 1],
					&container[(j - 1) + 1],
					1
				);
			}

			Array<Type>::Move(
				&value,
				&container[i],
				1
			);

			++this->size;

			return T_ITERATOR(
				&container[i]
			);
		}
		template<typename T_ITERATOR>
		T_ITERATOR Insert(T_ITERATOR it, const Type& value)
		{
			auto _it = Insert(
				it,
				Type(value)
			);

			return _it;
		}
		template<typename T_ITERATOR>
		T_ITERATOR Insert(T_ITERATOR it, const Type* lpValues, size_t count)
		{
			auto size     = GetSize();
			auto capacity = GetCapacity();
			auto newSize  = size + count;

			size_t i = &(*it) - &container[0];

			if (newSize > capacity)
			{

				Reserve(
					newSize - capacity
				);
			}

			for (size_t j = size; j > i; --j)
			{
				Array<Type>::Move(
					&container[j - 1],
					&container[(j - 1) + count],
					1
				);
			}

			Array<Type>::Copy(
				lpValues,
				&container[i],
				count
			);

			this->size = newSize;

			return T_ITERATOR(
				&container[i]
			);
		}

		virtual Iterator      begin() override
		{
			return Iterator(
				&container[0]
			);
		}
		virtual ConstIterator begin() const override
		{
			return ConstIterator(
				&container[0]
			);
		}

		virtual Iterator      end() override
		{
			return Iterator(
				&container[GetSize()]
			);
		}
		virtual ConstIterator end() const override
		{
			return ConstIterator(
				&container[GetSize()]
			);
		}

		virtual ConstIterator cbegin() const override
		{
			return ConstIterator(
				&container[0]
			);
		}

		virtual ConstIterator cend() const override
		{
			return ConstIterator(
				&container[GetSize()]
			);
		}

		virtual ReverseIterator      rbegin() override
		{
			return ReverseIterator(
				&container[GetSize() - 1]
			);
		}
		virtual ConstReverseIterator rbegin() const override
		{
			return ConstReverseIterator(
				&container[GetSize() - 1]
			);
		}

		virtual ReverseIterator      rend() override
		{
			return ReverseIterator(
				&container[-1]
			);
		}
		virtual ConstReverseIterator rend() const override
		{
			return ConstReverseIterator(
				&container[-1]
			);
		}

		virtual ConstReverseIterator crbegin() const override
		{
			return ConstReverseIterator(
				&container[GetSize() - 1]
			);
		}

		virtual ConstReverseIterator crend() const override
		{
			return ConstReverseIterator(
				&container[-1]
			);
		}

		Type&       operator [] (size_t index)
		{
			AL_ASSERT(
				index < GetSize(),
				"index out of bounds"
			);

			return container[index];
		}
		const Type& operator [] (size_t index) const
		{
			AL_ASSERT(
				index < GetSize(),
				"index out of bounds"
			);

			return container[index];
		}

		ArrayList& operator = (ArrayList&& arrayList)
		{
			container = Move(
				arrayList.container
			);

			size = arrayList.size;
			arrayList.size = 0;

			return *this;
		}
		ArrayList& operator = (const ArrayList& arrayList)
		{
			container = arrayList.container;
			size      = arrayList.size;

			return *this;
		}

		template<size_t S>
		ArrayList& operator = (const Type(&values)[S])
		{
			container.Assign(
				values
			);

			size = S;

			return *this;
		}

		Bool operator == (const ArrayList& arrayList) const
		{
			size_t size;

			if ((size = GetSize()) != arrayList.GetSize())
			{

				return False;
			}

			if constexpr (Is_POD<Type>::Value)
			{
				if (!memcmp(&container[0], &arrayList.container[0], size))
				{

					return False;
				}
			}
			else
			{
				auto lpValues1 = &container[0];
				auto lpValues2 = &arrayList.container[0];

				for (size_t i = 0; i < size; ++i, ++lpValues1, ++lpValues2)
				{
					if (*lpValues1 != *lpValues2)
					{

						return False;
					}
				}
			}

			return True;
		}
		Bool operator != (const ArrayList& arrayList) const
		{
			if (operator==(arrayList))
			{

				return False;
			}

			return True;
		}
	};
}
