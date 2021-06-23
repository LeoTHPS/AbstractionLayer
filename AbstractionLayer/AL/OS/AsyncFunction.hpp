#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/Tuple.hpp"

namespace AL::OS
{
	template<typename F>
	class AsyncFunction;

	template<typename T, typename ... TArgs>
	class AsyncFunction<T(TArgs ...)>
	{
		typedef AL::Function<T(TArgs ...)> Function;
		typedef AL::Function<T()>          DetourFunction;

		Function function;

	public:
		// @throw AL::Exceptions::Exception
		template<typename F>
		static void Execute(F&& function, TArgs ... args)
		{
#if defined(AL_PLATFORM_LINUX)
			DetourFunction detour(
				[function = Move(function), args = Collections::Tuple<TArgs ...>(Move(args) ...)]()
				{
					return args.Invoke(
						function
					);
				}
			);

			try
			{
				Thread::StartAndDetach(
					Move(detour)
				);
			}
			catch (Exceptions::Exception& exception)
			{

				throw Exceptions::Exception(
					Move(exception),
					"Error starting Thread"
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			Execute(
				GetCurrentThread(),
				Move(function),
				Forward<TArgs>(args) ...
			);
#else
			throw Exceptions::NotImplementedException();
#endif
		}
#if defined(AL_PLATFORM_WINDOWS)
		// @throw AL::Exceptions::Exception
		template<typename F>
		static void Execute(HANDLE hThread, F&& function, TArgs ... args)
		{
			auto lpDetour = new DetourFunction(
				[function = Move(function), args = Collections::Tuple<TArgs ...>(Move(args) ...)]()
				{
					args.Invoke(
						function
					);
				}
			);

			if (!QueueUserAPC(&AsyncFunction::ApcProc, hThread, reinterpret_cast<ULONG_PTR>(lpDetour)))
			{
				delete lpDetour;

				throw Exceptions::SystemException(
					"QueueUserAPC"
				);
			}
		}
		// @throw AL::Exceptions::Exception
		template<typename F>
		static void Execute(const Thread& thread, F&& function, TArgs ... args)
		{
			Execute(
				thread.GetHandle(),
				Move(function),
				Forward<TArgs>(args) ...
			);
		}
#endif

		template<typename F>
		AsyncFunction(F&& function)
			: function(
				Move(function)
			)
		{
		}

		// @throw AL::Exceptions::Exception
		void operator () (TArgs ... args) const
		{
#if defined(AL_PLATFORM_LINUX)
			DetourFunction detour(
				[this, args = Collections::Tuple<TArgs ...>(Move(args) ...)]()
				{
					return args.Invoke(
						function
					);
				}
			);

			try
			{
				Thread::StartAndDetach(
					Move(detour)
				);
			}
			catch (Exceptions::Exception& exception)
			{

				throw Exceptions::Exception(
					Move(exception),
					"Error starting Thread"
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			operator()(
				::GetCurrentThread(),
				Forward<TArgs>(args) ...
			);
#else
			throw Exceptions::NotImplementedException();
#endif
		}
#if defined(AL_PLATFORM_WINDOWS)
		// @throw AL::Exceptions::Exception
		void operator () (HANDLE hThread, TArgs ... args) const
		{
			auto lpDetour = new DetourFunction(
				[this, args = Collections::Tuple<TArgs ...>(Move(args) ...)]()
				{
					args.Invoke(
						function
					);
				}
			);

			if (!QueueUserAPC(&AsyncFunction::ApcProc, hThread, reinterpret_cast<ULONG_PTR>(lpDetour)))
			{
				delete lpDetour;

				throw Exceptions::SystemException(
					"QueueUserAPC"
				);
			}
		}
		// @throw AL::Exceptions::Exception
		template<typename F>
		void operator () (const Thread& thread, TArgs ... args) const
		{
			operator()(
				reinterpret_cast<HANDLE>(thread.GetHandle()),
				Forward<TArgs>(args) ...
			);
		}
#endif

	private:
#if defined(AL_PLATFORM_WINDOWS)
		static VOID NTAPI ApcProc(ULONG_PTR param)
		{
			auto lpDetour = reinterpret_cast<DetourFunction*>(
				param
			);

			try
			{
				(*lpDetour)();
			}
			catch (Exceptions::Exception&)
			{
				delete lpDetour;

				throw;
			}

			delete lpDetour;
		}
#endif
	};
}
