#pragma once
#include "AL/Common.hpp"

#include "LinkedList.hpp"

namespace AL::Collections
{
	template<typename T>
	class UnorderedSet
		: public LinkedList<T>::Collection
	{
		LinkedList<T> container;

	public:
		typedef T                                         Type;

		typedef typename LinkedList<Type>::Collection     Collection;

		typedef typename Collection::Iterator             Iterator;
		typedef typename Collection::ConstIterator        ConstIterator;

		typedef typename Collection::ReverseIterator      ReverseIterator;
		typedef typename Collection::ConstReverseIterator ConstReverseIterator;

		UnorderedSet()
		{
		}

		UnorderedSet(UnorderedSet&& unorderedSet)
			: container(
				Move(unorderedSet.container)
			)
		{
		}
		UnorderedSet(const UnorderedSet& unorderedSet)
			: container(
				unorderedSet.container
			)
		{
		}

		virtual ~UnorderedSet()
		{
		}

		virtual size_t GetSize() const override
		{
			return container.GetSize();
		}

		virtual size_t GetCapacity() const override
		{
			return container.GetCapacity();
		}

		Bool Contains(const Type& value) const
		{
			if (!container.Contains(value))
			{

				return False;
			}

			return True;
		}

		Void Clear()
		{
			container.Clear();
		}

		Void Add(Type&& value)
		{
			if (!Contains(value))
			{
				container.PushBack(
					Move(value)
				);
			}
		}
		Void Add(const Type& value)
		{
			if (!Contains(value))
			{
				container.PushBack(
					value
				);
			}
		}

		Void Remove(const Type& value)
		{
			container.Remove(
				value
			);
		}

		template<typename T_ITERATOR>
		Void Erase(T_ITERATOR it)
		{
			container.Erase(
				it
			);
		}
		template<typename T_ITERATOR>
		Void Erase(T_ITERATOR first, T_ITERATOR last)
		{
			container.Erase(
				first,
				last
			);
		}

		virtual Iterator      begin() override
		{
			return container.begin();
		}
		virtual ConstIterator begin() const override
		{
			return container.begin();
		}

		virtual Iterator      end() override
		{
			return container.end();
		}
		virtual ConstIterator end() const override
		{
			return container.end();
		}

		virtual ConstIterator cbegin() const override
		{
			return container.cbegin();
		}

		virtual ConstIterator cend() const override
		{
			return container.cend();
		}

		virtual ReverseIterator      rbegin() override
		{
			return container.rbegin();
		}
		virtual ConstReverseIterator rbegin() const override
		{
			return container.rbegin();
		}

		virtual ReverseIterator      rend() override
		{
			return container.rend();
		}
		virtual ConstReverseIterator rend() const override
		{
			return container.rend();
		}

		virtual ConstReverseIterator crbegin() const override
		{
			return container.crbegin();
		}

		virtual ConstReverseIterator crend() const override
		{
			return container.crend();
		}

		UnorderedSet& operator = (UnorderedSet&& unorderedSet)
		{
			container = Move(
				unorderedSet.container
			);

			return *this;
		}
		UnorderedSet& operator = (const UnorderedSet& unorderedSet)
		{
			container = unorderedSet.container;

			return *this;
		}

		Bool operator == (const UnorderedSet& unorderedSet) const
		{
			if (GetSize() != unorderedSet.GetSize())
			{

				return False;
			}

			for (auto it1 = cbegin(), it2 = unorderedSet.cbegin(); it1 != cend(); ++it1, ++it2)
			{
				if (*it1 != *it2)
				{

					return False;
				}
			}

			return True;
		}
		Bool operator != (const UnorderedSet& unorderedSet) const
		{
			if (operator==(unorderedSet))
			{

				return False;
			}

			return True;
		}
	};
}
