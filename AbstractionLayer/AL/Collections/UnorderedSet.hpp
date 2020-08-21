#pragma once
#include "AL/Common.hpp"

#include <unordered_set>

namespace AL::Collections
{
	template<typename T>
	class UnorderedSet
	{
		typedef std::unordered_set<T> Container;

		Container container;

	public:
		typedef T Type;

		typedef typename Container::iterator Iterator;
		typedef typename Container::const_iterator ConstIterator;

		UnorderedSet()
		{
		}

		template<typename ITERATOR>
		UnorderedSet(ITERATOR first, ITERATOR last)
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

		void Swap(UnorderedSet& set)
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

		bool operator == (const UnorderedSet& set) const
		{
			return container == set.container;
		}
		bool operator != (const UnorderedSet& set) const
		{
			return !operator==(set);
		}
	};
}
