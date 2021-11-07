#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/Tuple.hpp"
#include "AL/Collections/LinkedList.hpp"
#include "AL/Collections/Dictionary.hpp"

namespace AL
{
	template<typename F>
	class ScheduledEvent;

	template<typename F>
	using ScheduledEventHandler = EventHandler<F>;

	template<typename T, typename ... TArgs>
	class ScheduledEvent<T(TArgs ...)>
	{
		typedef AL::Event<T(TArgs ...)>                                        _Event;
		typedef AL::EventHandler<T(TArgs ...)>                                 _EventHandler;
		typedef Collections::Tuple<TArgs ...>                                  _EventHandlerArgs;
		typedef Collections::LinkedList<_EventHandlerArgs>                     _EventHandlerArgsLinkedList;
		typedef Collections::Dictionary<TimeSpan, _EventHandlerArgsLinkedList> _EventHandlerSchedule;

		_Event                event;
		_EventHandlerSchedule schedule;
		size_t                scheduleCount = 0;
		TimeSpan              accumulator;

	public:
		typedef _EventHandler Handler;

		ScheduledEvent()
		{
		}

		virtual ~ScheduledEvent()
		{
		}

		auto GetHandlerCount() const
		{
			return event.GetHandlerCount();
		}

		auto GetScheduleCount() const
		{
			return scheduleCount;
		}

		template<typename F>
		Void Register(F&& function)
		{
			event.Register(
				Move(function)
			);
		}
		Void Register(Handler&& handler)
		{
			event.Register(
				Move(handler)
			);
		}
		Void Register(const Handler& handler)
		{
			event.Register(
				handler
			);
		}
		Void Register(T(*lpFunction)(TArgs ...))
		{
			event.Register(
				lpFunction
			);
		}
		template<typename C>
		Void Register(T(C::*lpFunction)(TArgs ...), C& instance)
		{
			event.Register(
				lpFunction,
				instance
			);
		}
		template<typename C>
		Void Register(T(C::*lpFunction)(TArgs ...) const, C& instance)
		{
			event.Register(
				lpFunction,
				instance
			);
		}

		template<EventPriorities PRIORITY, typename F>
		Void Register(F&& function)
		{
			event.Register<PRIORITY>(
				Move(function)
			);
		}
		template<EventPriorities PRIORITY>
		Void Register(Handler&& handler)
		{
			event.Register<PRIORITY>(
				Move(handler)
			);
		}
		template<EventPriorities PRIORITY>
		Void Register(const Handler& handler)
		{
			event.Register<PRIORITY>(
				handler
			);
		}
		template<EventPriorities PRIORITY>
		Void Register(T(*lpFunction)(TArgs ...))
		{
			event.Register<PRIORITY>(
				lpFunction
			);
		}
		template<EventPriorities PRIORITY, typename C>
		Void Register(T(C::*lpFunction)(TArgs ...), C& instance)
		{
			event.Register<PRIORITY>(
				lpFunction,
				instance
			);
		}
		template<EventPriorities PRIORITY, typename C>
		Void Register(T(C::*lpFunction)(TArgs ...) const, C& instance)
		{
			event.Register<PRIORITY>(
				lpFunction,
				instance
			);
		}

		template<typename F>
		Bool Unregister(const F& function)
		{
			if (!event.Unregister(function))
			{

				return False;
			}

			return True;
		}
		Bool Unregister(const Handler& handler)
		{
			if (!event.Unregister(handler))
			{

				return False;
			}

			return True;
		}
		Bool Unregister(T(*lpFunction)(TArgs ...))
		{
			if (!event.Unregister(lpFunction))
			{

				return False;
			}

			return True;
		}
		template<typename C>
		Bool Unregister(T(C::*lpFunction)(TArgs ...), C& instance)
		{
			if (!event.Unregister(lpFunction, instance))
			{

				return False;
			}

			return True;
		}
		template<typename C>
		Bool Unregister(T(C::*lpFunction)(TArgs ...) const, C& instance)
		{
			if (!event.Unregister(lpFunction, instance))
			{

				return False;
			}

			return True;
		}

		template<EventPriorities PRIORITY, typename F>
		Bool Unregister(const F& function)
		{
			if (!event.template Unregister<PRIORITY>(function))
			{

				return False;
			}

			return True;
		}
		template<EventPriorities PRIORITY>
		Bool Unregister(const Handler& handler)
		{
			if (!event.template Unregister<PRIORITY>(handler))
			{

				return False;
			}

			return True;
		}
		template<EventPriorities PRIORITY>
		Bool Unregister(T(*lpFunction)(TArgs ...))
		{
			if (!event.template Unregister<PRIORITY>(lpFunction))
			{

				return False;
			}

			return True;
		}
		template<EventPriorities PRIORITY, typename C>
		Bool Unregister(T(C::*lpFunction)(TArgs ...), C& instance)
		{
			if (!event.template Unregister<PRIORITY>(lpFunction, instance))
			{

				return False;
			}

			return True;
		}
		template<EventPriorities PRIORITY, typename C>
		Bool Unregister(T(C::*lpFunction)(TArgs ...) const, C& instance)
		{
			if (!event.template Unregister<PRIORITY>(lpFunction, instance))
			{

				return False;
			}

			return True;
		}

		Void Clear()
		{
			ClearHandlers();
			ClearSchedule();
		}

		Void ClearHandlers()
		{
			event.Clear();
		}
		template<EventPriorities PRIORITY>
		Void ClearHandlers()
		{
			event.template Clear<PRIORITY>();
		}

		Void ClearSchedule()
		{
			schedule.Clear();
			scheduleCount = 0;
		}

		T Execute(TArgs ... args) const
		{
			event.Execute(
				Forward<TArgs>(args) ...
			);
		}
		template<EventPriorities PRIORITY>
		T Execute(TArgs ... args) const
		{
			event.template Execute<PRIORITY>(
				Forward<TArgs>(args) ...
			);
		}

		Void Post(TArgs ... args)
		{
			Schedule(
				TimeSpan::Zero,
				Forward<TArgs>(args) ...
			);
		}

		Void Schedule(TimeSpan delay, TArgs ... args)
		{
			schedule[accumulator + delay].EmplaceBack(
				Forward<TArgs>(args) ...
			);

			++scheduleCount;
		}

		Void Update(TimeSpan delta)
		{
			accumulator += delta;

			for (auto it1 = schedule.begin(); it1 != schedule.end(); )
			{
				if (it1->Key < accumulator)
				{

					break;
				}

				for (auto it2 = it1->Value.begin(); it2 != it1->Value.end(); )
				{
					it2->Invoke(
						Execute
					);

					it1->Value.Erase(
						it2++
					);

					--scheduleCount;
				}

				schedule.Erase(
					it1++
				);
			}
		}
	};

	template<typename T, typename ... TArgs>
	class ScheduledEvent<ScheduledEventHandler<T(TArgs ...)>>
		: public ScheduledEvent<T(TArgs ...)>
	{
	public:
		using ScheduledEvent<T(TArgs ...)>::ScheduledEvent;
	};
}
