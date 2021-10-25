#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/LinkedList.hpp"

namespace AL
{
	template<typename F>
	class Event;

	template<typename F>
	using EventHandler = Function<F>;

	template<typename T, typename ... TArgs>
	class Event<T(TArgs ...)>
	{
	public:
		typedef EventHandler<T(TArgs ...)> Handler;

		virtual ~Event()
		{
		}

		auto GetHandlerCount() const
		{
			return handlerRefs.GetSize();
		}

		template<typename F>
		Void Register(F&& handler)
		{
			Handler _handler(
				Move(handler)
			);

			Register(
				Move(_handler)
			);
		}
		Void Register(Handler&& handler)
		{
			handlers.PushBack(
				Move(handler)
			);

			handlerRefs.PushBack(
				&(*(--handlers.end()))
			);
		}
		Void Register(const Handler& handler)
		{
			handlerRefs.PushBack(
				&handler
			);
		}

		Void Unregister(const Handler& handler)
		{
			handlerRefs.Remove(
				&handler
			);
		}

		Void Clear()
		{
			handlers.Clear();
			handlerRefs.Clear();
		}

		T Execute(TArgs ... args) const
		{
			if constexpr (Is_Type<T, Void>::Value)
			{
				for (auto lpCallback : handlerRefs)
				{
					(*lpCallback)(
						Forward<TArgs>(args) ...
					);
				}
			}
			else
			{
				auto result = T();

				for (auto lpCallback : handlerRefs)
				{
					if (!(result = (*lpCallback)(Forward<TArgs>(args) ...)))
					{

						return result;
					}
				}

				return result;
			}
		}

	private:
		Collections::LinkedList<Handler>        handlers;
		Collections::LinkedList<const Handler*> handlerRefs;
	};

	template<typename T, typename ... TArgs>
	class Event<EventHandler<T(TArgs ...)>>
		: public Event<T(TArgs ...)>
	{
	public:
		using Event<T(TArgs ...)>::Event;
	};
}
