#pragma once
#include "AL/Common.hpp"

#include "Iterator.hpp"

namespace AL::Collections
{
	class ICollectionBase
	{
	public:
		ICollectionBase()
		{
		}

		virtual ~ICollectionBase()
		{
		}

		virtual Bool IsFull() const
		{
			return GetSize() == GetCapacity();
		}

		virtual Bool IsEmpty() const
		{
			return GetSize() == 0;
		}

		virtual size_t GetSize() const = 0;

		virtual size_t GetCapacity() const = 0;
	};

	template<typename T, template<typename> typename T_ITERATOR>
	class ICollection
		: public ICollectionBase
	{
	public:
		typedef T                          Type;

		typedef ICollection<T, T_ITERATOR> Collection;

		typedef T_ITERATOR<T>              Iterator;
		typedef T_ITERATOR<const T>        ConstIterator;

		ICollection()
		{
		}

		virtual ~ICollection()
		{
		}

		virtual Iterator begin() = 0;
		virtual ConstIterator begin() const = 0;

		virtual Iterator end() = 0;
		virtual ConstIterator end() const = 0;

		virtual ConstIterator cbegin() const = 0;

		virtual ConstIterator cend() const = 0;
	};

	template<typename T, template<typename> typename T_FORWARD_ITERATOR, template<typename> typename T_REVERSE_ITERATOR>
	class IBidirectionalCollection
		: public ICollectionBase
	{
	public:
		typedef T                                                                   Type;

		typedef IBidirectionalCollection<T, T_FORWARD_ITERATOR, T_REVERSE_ITERATOR> Collection;

		typedef T_FORWARD_ITERATOR<T>                                               Iterator;
		typedef T_FORWARD_ITERATOR<const T>                                         ConstIterator;

		typedef T_REVERSE_ITERATOR<T>                                               ReverseIterator;
		typedef T_REVERSE_ITERATOR<const T>                                         ConstReverseIterator;

		IBidirectionalCollection()
		{
		}

		virtual ~IBidirectionalCollection()
		{
		}

		virtual Iterator begin() = 0;
		virtual ConstIterator begin() const = 0;

		virtual Iterator end() = 0;
		virtual ConstIterator end() const = 0;

		virtual ConstIterator cbegin() const = 0;

		virtual ConstIterator cend() const = 0;

		virtual ReverseIterator rbegin() = 0;
		virtual ConstReverseIterator rbegin() const = 0;

		virtual ReverseIterator rend() = 0;
		virtual ConstReverseIterator rend() const = 0;

		virtual ConstReverseIterator crbegin() const = 0;

		virtual ConstReverseIterator crend() const = 0;
	};
}
