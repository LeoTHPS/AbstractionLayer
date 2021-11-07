#pragma once
#include "AL/Common.hpp"

#include "LinkedList.hpp"

namespace AL::Collections
{
	template<typename T>
	class Stack
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

		Stack()
		{
		}

		Stack(Stack&& stack)
			: container(
				Move(stack.container)
			)
		{
		}

		Stack(const Stack& stack)
			: container(
				stack.container
			)
		{
		}

		virtual ~Stack()
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

		Void Clear()
		{
			container.Clear();
		}

		Void Push(T&& value)
		{
			container.PushFront(
				Move(value)
			);
		}
		Void Push(const T& value)
		{
			container.PushFront(
				value
			);
		}

		Bool Pop()
		{
			if (container.GetSize() == 0)
			{

				return False;
			}

			container.PopFront();

			return True;
		}
		Bool Pop(T& value)
		{
			if (container.GetSize() == 0)
			{

				return False;
			}

			value = *container.begin();

			container.PopFront();

			return True;
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
	};
}
