#pragma once
#include "AL/Common.hpp"

#include <iterator>

namespace AL::Collections
{
	template<typename T>
	class ForwardIterator
	{
	public:
		typedef T*                        pointer;
		typedef T&                        reference;
		typedef T                         value_type;
		typedef std::ptrdiff_t            difference_type;
		typedef std::forward_iterator_tag iterator_category;

		ForwardIterator()
		{
		}

		virtual ~ForwardIterator()
		{
		}

		reference operator * () const;
		pointer   operator -> () const;

		ForwardIterator& operator ++ ();

		ForwardIterator operator ++ (int);

		Bool operator == (const ForwardIterator& it) const;
		Bool operator != (const ForwardIterator& it) const;
	};

	template<typename T>
	using Iterator_Is_Forward = Is_Type<typename T::iterator_category, std::forward_iterator_tag>;

	template<typename T>
	class RandomAccessIterator
	{
	public:
		typedef T*                              pointer;
		typedef T&                              reference;
		typedef T                               value_type;
		typedef std::ptrdiff_t                  difference_type;
		typedef std::random_access_iterator_tag iterator_category;

		RandomAccessIterator()
		{
		}

		virtual ~RandomAccessIterator()
		{
		}

		reference operator * () const;
		pointer   operator -> () const;
		
		RandomAccessIterator& operator ++ ();
		RandomAccessIterator& operator -- ();

		RandomAccessIterator operator ++ (int);
		RandomAccessIterator operator -- (int);

		RandomAccessIterator& operator += (size_t count);
		RandomAccessIterator& operator -= (size_t count);

		Bool operator == (const RandomAccessIterator& it) const;
		Bool operator != (const RandomAccessIterator& it) const;
	};

	template<typename T>
	using Iterator_Is_Random_Access = Is_Type<typename T::iterator_category, std::random_access_iterator_tag>;

	template<typename T>
	class BidirectionalIterator
	{
	public:
		typedef T*                              pointer;
		typedef T&                              reference;
		typedef T                               value_type;
		typedef std::ptrdiff_t                  difference_type;
		typedef std::bidirectional_iterator_tag iterator_category;

		BidirectionalIterator()
		{
		}

		virtual ~BidirectionalIterator()
		{
		}

		reference operator * () const;
		pointer   operator -> () const;

		BidirectionalIterator& operator ++ ();
		BidirectionalIterator& operator -- ();

		BidirectionalIterator operator ++ (int);
		BidirectionalIterator operator -- (int);

		Bool operator == (const BidirectionalIterator& it) const;
		Bool operator != (const BidirectionalIterator& it) const;
	};

	template<typename T>
	using Iterator_Is_Bidirectional = Is_Type<typename T::iterator_category, std::bidirectional_iterator_tag>;

	template<typename T>
	inline ssize_t GetIteratorDifference(const T& first, const T& last)
	{
		ssize_t difference = 0;

		if constexpr (Iterator_Is_Random_Access<T>::Value)
		{
			difference = reinterpret_cast<ssize_t>(last.operator->()) - reinterpret_cast<ssize_t>(first.operator->());
			difference /= sizeof(typename T::value_type);
		}
		else
		{
			for (auto it = first; it != last; ++it)
			{
				++difference;
			}
		}

		return difference;
	}
}
