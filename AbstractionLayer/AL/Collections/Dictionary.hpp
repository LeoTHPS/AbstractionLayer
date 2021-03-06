#pragma once
#include "AL/Common.hpp"

#include <map>

namespace AL::Collections
{
	template<typename TKey, typename TValue>
	class Dictionary
	{
		typedef std::map<TKey, TValue> Container;

		typedef typename Container::value_type Pair;

		Container container;

	public:
		typedef TKey Key;
		typedef TValue Value;

		typedef typename Container::iterator Iterator;
		typedef typename Container::const_iterator ConstIterator;

		typedef typename Container::reverse_iterator ReverseIterator;
		typedef typename Container::const_reverse_iterator ConstReverseIterator;

		Dictionary()
		{
		}

		Dictionary(Dictionary&& dictionary)
			: container(
				Move(dictionary.container)
			)
		{
		}

		Dictionary(const Dictionary& dictionary)
			: container(
				dictionary.container
			)
		{
		}

		template<typename ITERATOR>
		Dictionary(ITERATOR first, ITERATOR last)
			: container(
				first,
				last
			)
		{
		}

		virtual ~Dictionary()
		{
		}

		auto GetSize() const
		{
			return container.size();
		}

		bool Contains(const Key& key) const
		{
			return container.find(key) != container.end();
		}

		Iterator Find(const Key& key)
		{
			return container.find(
				key
			);
		}
		ConstIterator Find(const Key& key) const
		{
			return container.find(
				key
			);
		}

		void Clear()
		{
			container.clear();
		}

		void Swap(Dictionary& dictionary)
		{
			container.swap(
				dictionary.container
			);
		}

		Iterator Add(Key&& key, Value&& value)
		{
			return container.emplace(
				Move(key),
				Move(value)
			).first;
		}
		Iterator Add(const Key& key, Value&& value)
		{
			return container.emplace(
				key,
				Move(value)
			).first;
		}
		Iterator Add(const Key& key, const Value& value)
		{
			return container.emplace(
				key,
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

		void Remove(const Key& key)
		{
			container.erase(
				key
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

		auto& operator [] (Key&& key)
		{
			return container[Move(key)];
		}
		auto& operator [] (const Key& key)
		{
			return container[key];
		}
		auto& operator [] (const Key& key) const
		{
			return container[key];
		}

		auto& operator = (Dictionary&& dictionary)
		{
			container = Move(
				dictionary.container
			);

			return *this;
		}
		auto& operator = (const Dictionary& dictionary)
		{
			container = dictionary.container;

			return *this;
		}

		bool operator == (const Dictionary& dictionary) const
		{
			return container == dictionary.container;
		}
		bool operator != (const Dictionary& dictionary) const
		{
			return !operator==(dictionary);
		}
	};
}
