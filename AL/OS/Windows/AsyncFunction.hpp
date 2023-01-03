#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

#include "AL/Collections/Tuple.hpp"

#include "AL/OS/Thread.hpp"
#include "AL/OS/SystemException.hpp"

namespace AL::OS::Windows
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
		// @throw AL::Exception
		template<typename F>
		static Void Execute(F&& function, TArgs ... args)
		{
			Execute(
				::GetCurrentThread(),
				Move(function),
				Forward<TArgs>(args) ...
			);
		}
		// @throw AL::Exception
		template<typename F>
		static Void Execute(::HANDLE hThread, F&& function, TArgs ... args)
		{
			auto lpDetour = new DetourFunction(
				[function = Move(function), args = Collections::Tuple<TArgs ...>(Move(args) ...)]()
				{
					args.Invoke(
						function
					);
				}
			);

			if (!::QueueUserAPC(&AsyncFunction::ApcProc, hThread, reinterpret_cast<::ULONG_PTR>(lpDetour)))
			{
				delete lpDetour;

				throw SystemException(
					"QueueUserAPC"
				);
			}
		}
		// @throw AL::Exception
		template<typename F>
		static Void Execute(const Thread& thread, F&& function, TArgs ... args)
		{
			Execute(
				thread.GetHandle(),
				Move(function),
				Forward<TArgs>(args) ...
			);
		}

		template<typename F>
		AsyncFunction(F&& function)
			: function(
				Move(function)
			)
		{
		}

		// @throw AL::Exception
		Void operator () (TArgs ... args) const
		{
			operator()(
				::GetCurrentThread(),
				Forward<TArgs>(args) ...
			);
		}
		// @throw AL::Exception
		Void operator () (::HANDLE hThread, TArgs ... args) const
		{
			auto lpDetour = new DetourFunction(
				[this, args = Collections::Tuple<TArgs ...>(Move(args) ...)]()
				{
					args.Invoke(
						function
					);
				}
			);

			if (!::QueueUserAPC(&AsyncFunction::ApcProc, hThread, reinterpret_cast<::ULONG_PTR>(lpDetour)))
			{
				delete lpDetour;

				throw SystemException(
					"QueueUserAPC"
				);
			}
		}
		// @throw AL::Exception
		template<typename F>
		Void operator () (const Thread& thread, TArgs ... args) const
		{
			operator()(
				reinterpret_cast<::HANDLE>(thread.GetHandle()),
				Forward<TArgs>(args) ...
			);
		}

	private:
		static VOID NTAPI ApcProc(::ULONG_PTR param)
		{
			auto lpDetour = reinterpret_cast<DetourFunction*>(
				param
			);

			try
			{
				(*lpDetour)();
			}
			catch (Exception&)
			{
				delete lpDetour;

				throw;
			}

			delete lpDetour;
		}
	};
}
