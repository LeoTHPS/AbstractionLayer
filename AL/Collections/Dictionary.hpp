#pragma once
#include "AL/Common.hpp"

#include "LinkedList.hpp"
#include "KeyValuePair.hpp"

namespace AL::Collections
{
	template<typename T_KEY, typename T_VALUE>
	class Dictionary
		: public LinkedList<KeyValuePair<T_KEY, T_VALUE>>::Collection
	{
		typedef KeyValuePair<T_KEY, T_VALUE> _Pair;

		LinkedList<_Pair> container;

	public:
		typedef T_KEY                                     Key;
		typedef T_VALUE                                   Value;

		typedef KeyValuePair<Key, Value>                  Type;

		typedef typename LinkedList<Type>::Collection     Collection;

		typedef typename Collection::Iterator             Iterator;
		typedef typename Collection::ConstIterator        ConstIterator;

		typedef typename Collection::ReverseIterator      ReverseIterator;
		typedef typename Collection::ConstReverseIterator ConstReverseIterator;

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

		template<size_t S>
		Dictionary(const Type(&pairs)[S])
			: Dictionary(
				&pairs[0],
				S
			)
		{
		}
		Dictionary(const Type* lpPairs, size_t count)
		{
			for (size_t i = 0; i < count; ++i, ++lpPairs)
			{
				Add(
					lpPairs->Key,
					lpPairs->Value
				);
			}
		}

		virtual ~Dictionary()
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

		Iterator      Find(const Key& key)
		{
			for (auto it = begin(); it != end(); ++it)
			{
				if (it->Key == key)
				{

					return it;
				}
			}

			return end();
		}
		ConstIterator Find(const Key& key) const
		{
			for (auto it = cbegin(); it != cend(); ++it)
			{
				if (it->Key == key)
				{

					return it;
				}
			}

			return cend();
		}

		Bool Contains(const Key& key) const
		{
			for (auto& pair : container)
			{
				if (pair.Key == key)
				{

					return True;
				}
			}

			return False;
		}

		Void Clear()
		{
			container.Clear();
		}

		Void Add(Key&& key, Value&& value)
		{
			for (auto it = container.begin(); it != container.end(); ++it)
			{
				if (it->Key == key)
				{
					it->Value = Move(
						value
					);
					
					break;
				}

				if (it->Key > key)
				{
					container.Insert(
						it,
						KeyValuePair<Key, Value>
						{
							Move(key),
							Move(value)
						}
					);

					break;
				}
			}
		}
		Void Add(const Key& key, Value&& value)
		{
			Add(
				Key(key),
				Move(value)
			);
		}
		Void Add(const Key& key, const Value& value)
		{
			Add(
				Key(key),
				Value(value)
			);
		}

		Void Remove(const Key& key)
		{
			auto it = Find(
				key
			);

			if (it != end())
			{

				Erase(
					it
				);
			}
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

		Value&       operator [] (Key&& key)
		{
			Iterator it;

			for (it = container.begin(); it != container.end(); ++it)
			{
				if (it->Key == key)
				{

					break;
				}

				if (it->Key > key)
				{
					it = container.Insert(
						it,
						KeyValuePair<Key, Value>
						{
							Move(key),
							Value()
						}
					);

					break;
				}
			}

			return it->Value;
		}
		Value&       operator [] (const Key& key)
		{
			Iterator it;

			for (it = container.begin(); it != container.end(); ++it)
			{
				if (it->Key == key)
				{

					break;
				}

				if (it->Key > key)
				{
					it = container.Insert(
						it,
						KeyValuePair<Key, Value>
						{
							key,
							Value()
						}
					);

					break;
				}
			}

			return it->Value;
		}
		const Value& operator [] (const Key& key) const
		{
			auto it = Find(
				key
			);

			AL_ASSERT(
				it != end(),
				"key not found"
			);

			return it->Value;
		}

		Dictionary& operator = (Dictionary&& dictionary)
		{
			container = Move(
				dictionary.container
			);

			return *this;
		}
		Dictionary& operator = (const Dictionary& dictionary)
		{
			container = dictionary.container;

			return *this;
		}

		Bool operator == (const Dictionary& dictionary) const
		{
			if (GetSize() != dictionary.GetSize())
			{

				return False;
			}

			for (auto it1 = cbegin(), it2 = dictionary.cbegin(); it1 != cend(); ++it1, ++it2)
			{
				if (it1->Key != it2->Key)
				{

					return False;
				}

				if (it1->Value != it2->Value)
				{

					return False;
				}
			}

			return True;
		}
		Bool operator != (const Dictionary& dictionary) const
		{
			if (operator==(dictionary))
			{

				return False;
			}

			return True;
		}
	};
}
