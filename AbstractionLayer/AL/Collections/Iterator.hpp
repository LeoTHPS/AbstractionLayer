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

		reference operator * () const;
		reference operator -> () const;

		ForwardIterator& operator ++ ();

		ForwardIterator operator ++ (int);

		bool operator > (const ForwardIterator& it) const;
		bool operator < (const ForwardIterator& it) const;

		bool operator == (const ForwardIterator& it) const;
		bool operator != (const ForwardIterator& it) const;
	};
	
	template<typename T>
	class BidirectionalIterator
	{
	public:
		typedef T*                              pointer;
		typedef T&                              reference;
		typedef T                               value_type;
		typedef std::ptrdiff_t                  difference_type;
		typedef std::bidirectional_iterator_tag iterator_category;

		reference operator * () const;
		reference operator -> () const;

		BidirectionalIterator& operator ++ ();
		BidirectionalIterator& operator -- ();

		BidirectionalIterator operator ++ (int);
		BidirectionalIterator operator -- (int);

		bool operator > (const BidirectionalIterator& it) const;
		bool operator < (const BidirectionalIterator& it) const;

		bool operator == (const BidirectionalIterator& it) const;
		bool operator != (const BidirectionalIterator& it) const;
	};

	template<typename T>
	class RandomAccessIterator
	{
	public:
		typedef T*                              pointer;
		typedef T&                              reference;
		typedef T                               value_type;
		typedef std::ptrdiff_t                  difference_type;
		typedef std::random_access_iterator_tag iterator_category;

		reference operator * () const;
		reference operator -> () const;
		
		RandomAccessIterator& operator ++ ();
		RandomAccessIterator& operator -- ();

		RandomAccessIterator operator ++ (int);
		RandomAccessIterator operator -- (int);

		RandomAccessIterator& operator += (size_t offset);
		RandomAccessIterator& operator -= (size_t offset);

		bool operator > (const RandomAccessIterator& it) const;
		bool operator < (const RandomAccessIterator& it) const;

		bool operator == (const RandomAccessIterator& it) const;
		bool operator != (const RandomAccessIterator& it) const;
	};

	template<typename T>
	inline auto GetIteratorDifference(const T& first, const T& last)
	{
		int32 difference = 0;

		for (auto it = first; it != last; ++it)
		{
			++difference;
		}

		return difference;
	}
}
