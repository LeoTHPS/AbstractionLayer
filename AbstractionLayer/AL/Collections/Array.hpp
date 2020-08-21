#pragma once
#include "AL/Common.hpp"

#include <array>
#include <vector>

namespace AL::Collections
{
	template<typename T>
	class Array
	{
		typedef std::vector<T> Container;

		Container container;

	public:
		typedef T Type;

		typedef typename Container::iterator Iterator;
		typedef typename Container::const_iterator ConstIterator;

		typedef typename Container::reverse_iterator ReverseIterator;
		typedef typename Container::const_reverse_iterator ConstReverseIterator;

		Array()
			: container(
				0
			)
		{
		}

		explicit Array(size_t capacity)
			: container(
				capacity
			)
		{
		}

		template<size_t SIZE>
		Array(Type(&&values)[SIZE])
			: container(
				values
			)
		{
		}

		template<size_t SIZE>
		Array(const Type(&values)[SIZE])
			: container(
				values
			)
		{
		}

		template<typename ITERATOR>
		Array(ITERATOR first, ITERATOR last)
			: container(
				first,
				last
			)
		{
		}

		Array(const Type* lpValues, size_t count)
			: Array(
				count
			)
		{
			if constexpr (Is_POD<Type>::Value)
			{
				memcpy(
					&container[0],
					lpValues,
					count * sizeof(Type)
				);
			}
			else
			{
				for (size_t i = 0; i < count; ++i)
				{
					container[i] = Move(
						lpValues[i]
					);
				}
			}
		}

		auto GetSize() const
		{
			return container.size();
		}

		auto GetCapacity() const
		{
			return container.capacity();
		}

		void Clear()
		{
			container.clear();
		}

		void ShrinkToFit()
		{
			container.shrink_to_fit();
		}

		void Swap(Array& array)
		{
			container.swap(
				array.container
			);
		}

		void Reserve(size_t size)
		{
			container.reserve(
				GetSize() + size
			);
		}

		template<typename ITERATOR>
		void Erase(ITERATOR it)
		{
			container.erase(
				it
			);
		}
		template<typename ITERATOR>
		void Erase(ITERATOR first, ITERATOR last)
		{
			container.erase(
				first,
				last
			);
		}

		void PopBack()
		{
			container.pop_back();
		}

		void PushBack(Type&& value)
		{
			container.push_back(
				Move(value)
			);
		}
		void PushBack(const Type& value)
		{
			container.push_back(
				value
			);
		}

		template<typename ... TArgs>
		void EmplaceBack(TArgs ... args)
		{
			container.emplace_back(
				Forward<TArgs>(args) ...
			);
		}

		template<typename ITERATOR>
		void Insert(ITERATOR it, Type&& value)
		{
			container.insert(
				it,
				Move(value)
			);
		}
		template<typename ITERATOR>
		void Insert(ITERATOR it, const Type& value)
		{
			container.insert(
				it,
				value
			);
		}
		template<typename ITERATOR>
		void Insert(ITERATOR it, ITERATOR first, ITERATOR last)
		{
			container.insert(
				it,
				first,
				last
			);
		}

		void SetSize(size_t size)
		{
			container.resize(
				size
			);
		}

		void SetCapacity(size_t capacity)
		{
			container.reserve(
				capacity
			);
		}

		Iterator begin()
		{
			return container.begin();
		}
		ConstIterator begin() const
		{
			return container.begin();
		}

		Iterator end()
		{
			return container.end();
		}
		ConstIterator end() const
		{
			return container.end();
		}

		ConstIterator cbegin() const
		{
			return container.cbegin();
		}

		ConstIterator cend() const
		{
			return container.cend();
		}

		ReverseIterator rbegin()
		{
			return container.rbegin();
		}
		ConstReverseIterator rbegin() const
		{
			return container.rbegin();
		}

		ReverseIterator rend()
		{
			return container.rend();
		}
		ConstReverseIterator rend() const
		{
			return container.rend();
		}

		auto& operator [] (size_t index)
		{
			return container[index];
		}
		auto& operator [] (size_t index) const
		{
			return container[index];
		}

		bool operator == (const Array& array) const
		{
			return container == array.container;
		}
		bool operator != (const Array& array) const
		{
			return !operator==(array);
		}
	};

	template<typename T, size_t SIZE>
	class Array<T[SIZE]>
	{
		typedef std::array<T, SIZE> Container;

		Container container;

	public:
		typedef T Type;

		typedef typename Container::iterator Iterator;
		typedef typename Container::const_iterator ConstIterator;

		typedef typename Container::reverse_iterator ReverseIterator;
		typedef typename Container::const_reverse_iterator ConstReverseIterator;

		Array()
		{
		}

		Array(Type(&&values)[SIZE])
		{
			for (size_t i = 0; i < SIZE; ++i)
			{
				container[i] = Move(
					values[i]
				);
			}
		}

		Array(const Type(&values)[SIZE])
		{
			for (size_t i = 0; i < SIZE; ++i)
			{
				container[i] = values[i];
			}
		}
		
		static constexpr auto GetSize()
		{
			return SIZE;
		}

		static constexpr auto GetCapacity()
		{
			return SIZE;
		}

		void Swap(Array& array)
		{
			container.swap(
				array.container
			);
		}

		void Fill(const Type& value)
		{
			container.fill(
				value
			);
		}

		Iterator begin()
		{
			return container.begin();
		}
		ConstIterator begin() const
		{
			return container.begin();
		}

		Iterator end()
		{
			return container.end();
		}
		ConstIterator end() const
		{
			return container.end();
		}

		ConstIterator cbegin() const
		{
			return container.cbegin();
		}

		ConstIterator cend() const
		{
			return container.cend();
		}

		ReverseIterator rbegin()
		{
			return container.rbegin();
		}
		ConstReverseIterator rbegin() const
		{
			return container.crbegin();
		}

		ReverseIterator rend()
		{
			return container.rend();
		}
		ConstReverseIterator rend() const
		{
			return container.crend();
		}

		auto& operator [] (size_t index)
		{
			return container[index];
		}
		auto& operator [] (size_t index) const
		{
			return container[index];
		}

		bool operator == (const Array& array) const
		{
			return container == array.container;
		}
		bool operator != (const Array& array) const
		{
			return !operator==(array);
		}
	};
}
