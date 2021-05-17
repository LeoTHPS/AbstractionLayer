#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

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
		template<typename F>
		AsyncFunction(F&& function)
			: function(
				Move(function)
			)
		{
		}

		// @throw AL::Exception
		void operator () (TArgs ... args) const
		{
			operator()(
				::GetCurrentThread(),
				Forward<TArgs>(args) ...
			);
		}
		// @throw AL::Exception
		void operator () (HANDLE hThread, TArgs ... args) const
		{
			auto lpDetour = new DetourFunction(
				[this, args = Move(args) ...]()
				{
					return function(
						Forward<TArgs>(args) ...
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
		// @throw AL::Exception
		template<typename F>
		void operator () (const Thread& thread, TArgs ... args) const
		{
			operator()(
				reinterpret_cast<HANDLE>(thread.GetHandle()),
				Forward<TArgs>(args) ...
			);
		}

	private:
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
	};
}
