#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/LinkedList.hpp"

namespace AL
{
	template<typename F>
	class Event;

	template<typename F>
	using EventHandler = Function<F>;

	enum class EventPriorities : uint8
	{
		Lowest,
		Low,
		Standard,
		High,
		Highest
	};

	template<typename T, typename ... TArgs>
	class Event<T(TArgs ...)>
	{
		typedef EventHandler<T(TArgs ...)> _Handler;

		class HandlerContext
		{
			_Handler*       lpAllocatedHandler;
			const _Handler* lpHandler;

		public:
			HandlerContext()
				: lpAllocatedHandler(
					nullptr
				),
				lpHandler(
					nullptr
				)
			{
			}

			template<typename F>
			explicit HandlerContext(F&& handler)
				: lpAllocatedHandler(
					new _Handler(
						Move(handler)
					)
				),
				lpHandler(
					lpAllocatedHandler
				)
			{
			}
			explicit HandlerContext(_Handler&& handler)
				: lpAllocatedHandler(
					new _Handler(
						Move(handler)
					)
				),
				lpHandler(
					lpAllocatedHandler
				)
			{
			}
			explicit HandlerContext(const _Handler& handler)
				: lpAllocatedHandler(
					nullptr
				),
				lpHandler(
					&handler
				)
			{
			}

			HandlerContext(HandlerContext&& handlerContext)
				: lpAllocatedHandler(
					handlerContext.lpAllocatedHandler
				),
				lpHandler(
					handlerContext.lpHandler
				)
			{
				handlerContext.lpAllocatedHandler = nullptr;
				handlerContext.lpHandler          = nullptr;
			}

			HandlerContext(const HandlerContext& handlerContext)
				: lpAllocatedHandler(
					(handlerContext.lpAllocatedHandler != nullptr) ? new _Handler(*handlerContext.lpAllocatedHandler) : nullptr
				),
				lpHandler(
					(handlerContext.lpAllocatedHandler != nullptr) ? lpAllocatedHandler : handlerContext.lpHandler
				)
			{
			}

			virtual ~HandlerContext()
			{
				if (lpAllocatedHandler != nullptr)
				{
					delete lpAllocatedHandler;
				}
			}

			T Execute(TArgs ... args) const
			{
				return (*lpHandler)(
					Forward<TArgs>(args) ...
				);
			}

			Bool Compare(const _Handler& handler) const
			{
				if (lpHandler != &handler)
				{

					return False;
				}

				return True;
			}

			HandlerContext& operator = (HandlerContext&& handlerContext)
			{
				if (lpAllocatedHandler != nullptr)
				{

					delete lpAllocatedHandler;
				}

				lpAllocatedHandler = handlerContext.lpAllocatedHandler;
				handlerContext.lpAllocatedHandler = nullptr;

				lpHandler = handlerContext.lpHandler;
				handlerContext.lpHandler = nullptr;

				return *this;
			}
			HandlerContext& operator = (const HandlerContext& handlerContext)
			{
				if (lpAllocatedHandler != nullptr)
				{

					delete lpAllocatedHandler;
				}

				lpAllocatedHandler = (handlerContext.lpAllocatedHandler != nullptr) ? new _Handler(*handlerContext.lpAllocatedHandler) : nullptr;
				lpHandler          = (handlerContext.lpAllocatedHandler != nullptr) ? lpAllocatedHandler : handlerContext.lpHandler;

				return *this;
			}

			Bool operator == (const HandlerContext& handlerContext) const
			{
				if (lpHandler != handlerContext.lpHandler)
				{

					return False;
				}

				return True;
			}
			Bool operator != (const HandlerContext& handlerContext) const
			{
				if (operator==(handlerContext))
				{

					return False;
				}

				return True;
			}
		};

		typedef Collections::LinkedList<HandlerContext> HandlerContextList;

		HandlerContextList handlers_Lowest;
		HandlerContextList handlers_Low;
		HandlerContextList handlers_Standard;
		HandlerContextList handlers_High;
		HandlerContextList handlers_Highest;

	public:
		typedef _Handler Handler;

		Event()
		{
		}

		virtual ~Event()
		{
		}

		auto GetHandlerCount() const
		{
			return
				GetHandlers<EventPriorities::Lowest>().GetSize() +
				GetHandlers<EventPriorities::Low>().GetSize() +
				GetHandlers<EventPriorities::Standard>().GetSize() +
				GetHandlers<EventPriorities::High>().GetSize() +
				GetHandlers<EventPriorities::Highest>().GetSize();
		}

		template<typename F>
		Void Register(F&& handler)
		{
			Register<EventPriorities::Standard>(
				Move(handler)
			);
		}
		Void Register(Handler&& handler)
		{
			Register<EventPriorities::Standard>(
				Move(handler)
			);
		}
		Void Register(const Handler& handler)
		{
			Register<EventPriorities::Standard>(
				handler
			);
		}

		template<EventPriorities PRIORITY, typename F>
		Void Register(F&& handler)
		{
			GetHandlers<PRIORITY>().EmplaceBack(
				Move(handler)
			);
		}
		template<EventPriorities PRIORITY>
		Void Register(Handler&& handler)
		{
			GetHandlers<PRIORITY>().EmplaceBack(
				Move(handler)
			);
		}
		template<EventPriorities PRIORITY>
		Void Register(const Handler& handler)
		{
			GetHandlers<PRIORITY>().EmplaceBack(
				handler
			);
		}

		Bool Unregister(const Handler& handler)
		{
			if (Unregister<EventPriorities::Lowest>(handler))
			{

				return True;
			}

			if (Unregister<EventPriorities::Low>(handler))
			{

				return True;
			}

			if (Unregister<EventPriorities::Standard>(handler))
			{

				return True;
			}

			if (Unregister<EventPriorities::High>(handler))
			{

				return True;
			}

			if (Unregister<EventPriorities::Highest>(handler))
			{

				return True;
			}

			return False;
		}
		template<EventPriorities PRIORITY>
		Bool Unregister(const Handler& handler)
		{
			auto& handlers = GetHandlers<PRIORITY>();

			for (auto it = handlers.rbegin(); it != handlers.rend(); ++it)
			{
				if (it->Compare(handler))
				{
					handlers.Erase(
						it
					);

					return True;
				}
			}

			return False;
		}

		Void Clear()
		{
			Clear<EventPriorities::Lowest>();
			Clear<EventPriorities::Low>();
			Clear<EventPriorities::Standard>();
			Clear<EventPriorities::High>();
			Clear<EventPriorities::Highest>();
		}
		template<EventPriorities PRIORITY>
		Void Clear()
		{
			GetHandlers<PRIORITY>().Clear();
		}

		T Execute(TArgs ... args) const
		{
			if constexpr (Is_Type<T, Void>::Value)
			{
				Execute<EventPriorities::Highest>(Forward<TArgs>(args) ...);
				Execute<EventPriorities::High>(Forward<TArgs>(args) ...);
				Execute<EventPriorities::Standard>(Forward<TArgs>(args) ...);
				Execute<EventPriorities::Low>(Forward<TArgs>(args) ...);
				Execute<EventPriorities::Lowest>(Forward<TArgs>(args) ...);
			}
			else if constexpr (Is_Type<T, Bool>::Value)
			{
				if (!Execute<EventPriorities::Highest>(Forward<TArgs>(args) ...))
				{

					return False;
				}

				if (!Execute<EventPriorities::High>(Forward<TArgs>(args) ...))
				{

					return False;
				}

				if (!Execute<EventPriorities::Standard>(Forward<TArgs>(args) ...))
				{

					return False;
				}

				if (!Execute<EventPriorities::Low>(Forward<TArgs>(args) ...))
				{

					return False;
				}

				if (!Execute<EventPriorities::Lowest>(Forward<TArgs>(args) ...))
				{

					return False;
				}

				return True;
			}
		}
		template<EventPriorities PRIORITY>
		T Execute(TArgs ... args) const
		{
			if constexpr (Is_Type<T, Void>::Value)
			{
				for (auto& handlerContext : GetHandlers<PRIORITY>())
				{
					handlerContext.Execute(
						Forward<TArgs>(args) ...
					);
				}
			}
			else if constexpr (Is_Type<T, Bool>::Value)
			{
				for (auto& handlerContext : GetHandlers<PRIORITY>())
				{
					if (!handlerContext.Execute(Forward<TArgs>(args) ...))
					{

						return False;
					}
				}

				return True;
			}
		}

	private:
		template<EventPriorities PRIORITY>
		HandlerContextList& GetHandlers()
		{
			if      constexpr (PRIORITY == EventPriorities::Lowest)
			{
				return handlers_Lowest;
			}
			else if constexpr (PRIORITY == EventPriorities::Low)
			{
				return handlers_Low;
			}
			else if constexpr (PRIORITY == EventPriorities::Standard)
			{
				return handlers_Standard;
			}
			else if constexpr (PRIORITY == EventPriorities::High)
			{
				return handlers_High;
			}
			else if constexpr (PRIORITY == EventPriorities::Highest)
			{
				return handlers_Highest;
			}
		}
		template<EventPriorities PRIORITY>
		const HandlerContextList& GetHandlers() const
		{
			if      constexpr (PRIORITY == EventPriorities::Lowest)
			{
				return handlers_Lowest;
			}
			else if constexpr (PRIORITY == EventPriorities::Low)
			{
				return handlers_Low;
			}
			else if constexpr (PRIORITY == EventPriorities::Standard)
			{
				return handlers_Standard;
			}
			else if constexpr (PRIORITY == EventPriorities::High)
			{
				return handlers_High;
			}
			else if constexpr (PRIORITY == EventPriorities::Highest)
			{
				return handlers_Highest;
			}
		}
	};

	template<typename T, typename ... TArgs>
	class Event<EventHandler<T(TArgs ...)>>
		: public Event<T(TArgs ...)>
	{
	public:
		using Event<T(TArgs ...)>::Event;
	};
}
