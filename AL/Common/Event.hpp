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

		enum class _HandlerTypes
		{
			Lambda,
			Handler,
			HandlerPtr,
			Function,
			MemberFunction,
			MemberFunctionConst
		};

		struct _HandlerContext
		{
			_HandlerTypes       Type;

			_Handler            Handler;                                 // Lambda/Handler/Function/MemberFunction/MemberFunctionConst
			const _Handler*     lpHandler;                               // HandlerPtr
			Void*               lpInstance;                              // MemberFunction/MemberFunctionConst
			TypeHash            LambdaHash;                              // Lambda
			T(*                 lpFunction)(TArgs ...);                  // Function
			T(_HandlerContext::*lpMemberFunction)(TArgs ...);            // MemberFunction
			T(_HandlerContext::*lpMemberFunctionConst)(TArgs ...) const; // MemberFunctionConst
		};

		typedef Collections::LinkedList<_HandlerContext> _HandlerContextLinkedList;

		_HandlerContextLinkedList handlers_Lowest;
		_HandlerContextLinkedList handlers_Low;
		_HandlerContextLinkedList handlers_Standard;
		_HandlerContextLinkedList handlers_High;
		_HandlerContextLinkedList handlers_Highest;

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
		Void Register(F&& function)
		{
			Register<EventPriorities::Standard>(
				Move(function)
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
		Void Register(T(*lpFunction)(TArgs ...))
		{
			Register<EventPriorities::Standard>(
				lpFunction
			);
		}
		template<typename C>
		Void Register(T(C::*lpFunction)(TArgs ...), C& instance)
		{
			Register<EventPriorities::Standard>(
				lpFunction,
				instance
			);
		}
		template<typename C>
		Void Register(T(C::*lpFunction)(TArgs ...) const, C& instance)
		{
			Register<EventPriorities::Standard>(
				lpFunction,
				instance
			);
		}

		template<EventPriorities PRIORITY, typename F>
		Void Register(F&& function)
		{
			_HandlerContext context =
			{
				.Type       = _HandlerTypes::Lambda,
				.Handler    = _Handler(Move(function)),
				.LambdaHash = Type<F>::Hash
			};

			Register<PRIORITY>(
				Move(context)
			);
		}
		template<EventPriorities PRIORITY>
		Void Register(Handler&& handler)
		{
			_HandlerContext context =
			{
				.Type    = _HandlerTypes::Handler,
				.Handler = Move(handler)
			};

			Register<PRIORITY>(
				Move(context)
			);
		}
		template<EventPriorities PRIORITY>
		Void Register(const Handler& handler)
		{
			_HandlerContext context =
			{
				.Type      = _HandlerTypes::HandlerPtr,
				.lpHandler = &handler
			};

			Register<PRIORITY>(
				Move(context)
			);
		}
		template<EventPriorities PRIORITY>
		Void Register(T(*lpFunction)(TArgs ...))
		{
			_HandlerContext context =
			{
				.Type       = _HandlerTypes::Function,
				.lpFunction = lpFunction
			};

			Register<PRIORITY>(
				Move(context)
			);
		}
		template<EventPriorities PRIORITY, typename C>
		Void Register(T(C::*lpFunction)(TArgs ...), C& instance)
		{
			_HandlerContext context =
			{
				.Type             = _HandlerTypes::MemberFunction,
				.Handler          = _Handler(lpFunction, instance),
				.lpInstance       = &instance,
				.lpMemberFunction = reinterpret_cast<decltype(_HandlerContext::lpMemberFunction)>(lpFunction)
			};

			Register<PRIORITY>(
				Move(context)
			);
		}
		template<EventPriorities PRIORITY, typename C>
		Void Register(T(C::*lpFunction)(TArgs ...) const, C& instance)
		{
			_HandlerContext context =
			{
				.Type                  = _HandlerTypes::MemberFunctionConst,
				.Handler               = _Handler(lpFunction, instance),
				.lpInstance            = &instance,
				.lpMemberFunctionConst = reinterpret_cast<decltype(_HandlerContext::lpMemberFunctionConst)>(lpFunction)
			};

			Register<PRIORITY>(
				Move(context)
			);
		}

		template<typename F>
		Bool Unregister(const F& function)
		{
			if (Unregister<EventPriorities::Highest>(function))
			{

				return True;
			}

			if (Unregister<EventPriorities::High>(function))
			{

				return True;
			}

			if (Unregister<EventPriorities::Standard>(function))
			{

				return True;
			}

			if (Unregister<EventPriorities::Low>(function))
			{

				return True;
			}

			if (Unregister<EventPriorities::Lowest>(function))
			{

				return True;
			}

			return False;
		}
		Bool Unregister(const Handler& handler)
		{
			if (Unregister<EventPriorities::Highest>(handler))
			{

				return True;
			}

			if (Unregister<EventPriorities::High>(handler))
			{

				return True;
			}

			if (Unregister<EventPriorities::Standard>(handler))
			{

				return True;
			}

			if (Unregister<EventPriorities::Low>(handler))
			{

				return True;
			}

			if (Unregister<EventPriorities::Lowest>(handler))
			{

				return True;
			}

			return False;
		}
		Bool Unregister(T(*lpFunction)(TArgs ...))
		{
			if (Unregister<EventPriorities::Highest>(lpFunction))
			{

				return True;
			}

			if (Unregister<EventPriorities::High>(lpFunction))
			{

				return True;
			}

			if (Unregister<EventPriorities::Standard>(lpFunction))
			{

				return True;
			}

			if (Unregister<EventPriorities::Low>(lpFunction))
			{

				return True;
			}

			if (Unregister<EventPriorities::Lowest>(lpFunction))
			{

				return True;
			}

			return True;
		}
		template<typename C>
		Bool Unregister(T(C::*lpFunction)(TArgs ...), C& instance)
		{
			if (Unregister<EventPriorities::Highest>(lpFunction, instance))
			{

				return True;
			}

			if (Unregister<EventPriorities::High>(lpFunction, instance))
			{

				return True;
			}

			if (Unregister<EventPriorities::Standard>(lpFunction, instance))
			{

				return True;
			}

			if (Unregister<EventPriorities::Low>(lpFunction, instance))
			{

				return True;
			}

			if (Unregister<EventPriorities::Lowest>(lpFunction, instance))
			{

				return True;
			}

			return False;
		}
		template<typename C>
		Bool Unregister(T(C::*lpFunction)(TArgs ...) const, C& instance)
		{
			if (Unregister<EventPriorities::Highest>(lpFunction, instance))
			{

				return True;
			}

			if (Unregister<EventPriorities::High>(lpFunction, instance))
			{

				return True;
			}

			if (Unregister<EventPriorities::Standard>(lpFunction, instance))
			{

				return True;
			}

			if (Unregister<EventPriorities::Low>(lpFunction, instance))
			{

				return True;
			}

			if (Unregister<EventPriorities::Lowest>(lpFunction, instance))
			{

				return True;
			}

			return False;
		}

		template<EventPriorities PRIORITY, typename F>
		Bool Unregister(const F& function)
		{
			auto function_Hash = Type<F>::Hash;

			return UnregisterIf([function_Hash](_HandlerContext& _context)
			{
				if (_context.Type != _HandlerTypes::Lambda)
				{

					return False;
				}

				if (_context.LambdaHash != function_Hash)
				{

					return False;
				}

				return True;
			});
		}
		template<EventPriorities PRIORITY>
		Bool Unregister(const Handler& handler)
		{
			auto handler_Hash = handler.GetHash();

			return UnregisterIf<PRIORITY>([handler_Hash](_HandlerContext& _context)
			{
				switch (_context.Type)
				{
					case _HandlerTypes::Handler:
					{
						if (_context.Handler.GetHash() != handler_Hash)
						{

							return False;
						}
					}
					return True;

					case _HandlerTypes::HandlerPtr:
					{
						if (_context.lpHandler->GetHash() != handler_Hash)
						{

							return False;
						}
					}
					return True;
				}

				return False;
			});
		}
		template<EventPriorities PRIORITY>
		Bool Unregister(T(*lpFunction)(TArgs ...))
		{
			return UnregisterIf<PRIORITY>([lpFunction](_HandlerContext& _context)
			{
				if (_context.Type != _HandlerTypes::Function)
				{

					return False;
				}

				if (_context.lpFunction != lpFunction)
				{

					return False;
				}

				return False;
			});
		}
		template<EventPriorities PRIORITY, typename C>
		Bool Unregister(T(C::*lpFunction)(TArgs ...), C& instance)
		{
			return UnregisterIf<PRIORITY>([lpFunction = reinterpret_cast<decltype(_HandlerContext::lpMemberFunction)>(lpFunction), lpInstance = static_cast<Void*>(&instance)](_HandlerContext& _context)
			{
				if (_context.Type != _HandlerTypes::MemberFunction)
				{

					return False;
				}

				if (_context.lpMemberFunction != lpFunction)
				{

					return False;
				}

				if (_context.lpInstance != lpInstance)
				{

					return False;
				}

				return True;
			});
		}
		template<EventPriorities PRIORITY, typename C>
		Bool Unregister(T(C::*lpFunction)(TArgs ...) const, C& instance)
		{
			return UnregisterIf<PRIORITY>([lpFunction = reinterpret_cast<decltype(_HandlerContext::lpMemberFunctionConst)>(lpFunction), lpInstance = static_cast<Void*>(&instance)](_HandlerContext& _context)
			{
				if (_context.Type != _HandlerTypes::MemberFunctionConst)
				{

					return False;
				}

				if (_context.lpMemberFunctionConst != lpFunction)
				{

					return False;
				}

				if (_context.lpInstance != lpInstance)
				{

					return False;
				}

				return True;
			});
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
					Execute(
						handlerContext,
						Forward<TArgs>(args) ...
					);
				}
			}
			else if constexpr (Is_Type<T, Bool>::Value)
			{
				for (auto& handlerContext : GetHandlers<PRIORITY>())
				{
					if (!Execute(handlerContext, Forward<TArgs>(args) ...))
					{

						return False;
					}
				}

				return True;
			}
		}

	private:
		template<EventPriorities PRIORITY>
		_HandlerContextLinkedList& GetHandlers()
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
		const _HandlerContextLinkedList& GetHandlers() const
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
		Void Register(_HandlerContext&& context)
		{
			GetHandlers<PRIORITY>().PushBack(
				Move(context)
			);
		}

		template<EventPriorities PRIORITY, typename F>
		Bool UnregisterIf(F&& function)
		{
			auto& handlers = GetHandlers<PRIORITY>();

			for (auto it = handlers.rbegin(); it != handlers.rend(); ++it)
			{
				if (function(*it))
				{
					handlers.Erase(
						it
					);

					return True;
				}
			}

			return False;
		}

		T Execute(const _HandlerContext& context, TArgs ... args) const
		{
			switch (context.Type)
			{
				case _HandlerTypes::Function:
					return context.lpFunction(Forward<TArgs>(args) ...);

				case _HandlerTypes::HandlerPtr:
					return (*context.lpHandler)(Forward<TArgs>(args) ...);

				default:
					return context.Handler(Forward<TArgs>(args) ...);
			}

			throw NotImplementedException();
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
