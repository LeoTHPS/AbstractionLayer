#pragma once
#include "AL/Common.hpp"

#include <stack>

namespace AL::Collections
{
	template<typename T>
	class Stack
	{
		std::stack<T> container;

	public:
		typedef T Type;

		auto GetSize() const
		{
			return container.size();
		}

		auto GetTop()
		{
			return GetSize() ? &container.top() : nullptr;
		}
		auto GetTop() const
		{
			return GetSize() ? &container.top() : nullptr;
		}

		void Clear()
		{
			if (auto size = GetSize())
			{
				for (size_t i = 0; i < size; ++i)
				{
					container.pop();
				}
			}
		}

		void Swap(Stack& stack)
		{
			container.swap(
				stack.container
			);
		}

		void Push(Type&& value)
		{
			container.push(
				Move(value)
			);
		}
		void Push(const Type& value)
		{
			container.push(
				value
			);
		}

		template<typename ... TArgs>
		void Emplace(TArgs ... args)
		{
			container.emplace(
				Forward<TArgs>(args) ...
			);
		}

		bool Pop()
		{
			if (GetSize())
			{
				container.pop();

				return true;
			}

			return false;
		}
		bool Pop(Type& value)
		{
			if (GetSize())
			{
				value = Move(
					container.top()
				);

				container.pop();

				return true;
			}

			return false;
		}
	};
}
