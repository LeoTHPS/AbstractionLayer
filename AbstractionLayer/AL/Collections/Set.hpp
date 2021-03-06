#pragma once
#include "AL/Common.hpp"

#include <set>

namespace AL::Collections
{
	template<typename T>
	class Set
	{
		typedef std::set<T> Container;

		Container container;

	public:
		typedef T Type;

		typedef typename Container::iterator Iterator;
		typedef typename Container::const_iterator ConstIterator;

		typedef typename Container::reverse_iterator ReverseIterator;
		typedef typename Container::const_reverse_iterator ConstReverseIterator;

		Set()
		{
		}

		Set(Set&& set)
			: container(
				Move(set.container)
			)
		{
		}

		Set(const Set& set)
			: container(
				set.container
			)
		{
		}

		template<typename ITERATOR>
		Set(ITERATOR first, ITERATOR last)
			: container(
				first,
				last
			)
		{
		}

		virtual ~Set()
		{
		}

		auto GetSize() const
		{
			return container.size();
		}

		bool Contains(const Type& value) const
		{
			return container.find(value) != container.end();
		}

		Iterator Find(const Type& value)
		{
			return container.find(
				value
			);
		}
		ConstIterator Find(const Type& value) const
		{
			return container.find(
				value
			);
		}

		void Clear()
		{
			container.clear();
		}

		void Swap(Set& set)
		{
			container.swap(
				set.container
			);
		}

		Iterator Add(Type&& value)
		{
			return container.emplace(
				Move(value)
			).first;
		}
		Iterator Add(const Type& value)
		{
			return container.emplace(
				value
			).first;
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

		void Remove(const Type& value)
		{
			container.erase(
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

		auto& operator = (Set&& set)
		{
			container = Move(
				set.container
			);

			return *this;
		}
		auto& operator = (const Set& set)
		{
			container = set.container;

			return *this;
		}

		bool operator == (const Set& set) const
		{
			return container == set.container;
		}
		bool operator != (const Set& set) const
		{
			return !operator==(set);
		}
	};
}
