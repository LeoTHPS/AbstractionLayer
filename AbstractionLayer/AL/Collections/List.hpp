#pragma once
#include "AL/Common.hpp"

#include <list>

namespace AL::Collections
{
	template<typename T>
	class List
	{
		typedef std::list<T> Container;

		Container container;

	public:
		typedef T Type;

		typedef typename Container::iterator Iterator;
		typedef typename Container::const_iterator ConstIterator;

		typedef typename Container::reverse_iterator ReverseIterator;
		typedef typename Container::const_reverse_iterator ConstReverseIterator;

		List()
		{
		}

		template<typename ITERATOR>
		List(ITERATOR first, ITERATOR last)
			: container(
				first,
				last
			)
		{
		}

		auto GetSize() const
		{
			return container.size();
		}

		auto GetBack()
		{
			return GetSize() ? &container.back() : nullptr;
		}
		auto GetBack() const
		{
			return GetSize() ? &container.back() : nullptr;
		}

		auto GetFront()
		{
			return GetSize() ? &container.front() : nullptr;
		}
		auto GetFront() const
		{
			return GetSize() ? &container.front() : nullptr;
		}

		void Clear()
		{
			container.clear();
		}

		void PopBack()
		{
			container.pop_back();
		}

		void PopFront()
		{
			container.pop_front();
		}

		void Swap(List& list)
		{
			container.swap(
				list.container
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

		void Remove(const Type& value)
		{
			container.remove(
				value
			);
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

		void PushFront(Type&& value)
		{
			container.push_front(
				Move(value)
			);
		}
		void PushFront(const Type& value)
		{
			container.push_front(
				value
			);
		}

		template<typename ... TArgs>
		void EmplaceFront(TArgs ... args)
		{
			container.emplace_front(
				Forward<TArgs>(args) ...
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

		bool operator == (const List& list) const
		{
			return container == list.container;
		}
		bool operator != (const List& list) const
		{
			return !operator==(list);
		}
	};
}
