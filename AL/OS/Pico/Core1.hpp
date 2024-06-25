#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_PICO)
	#error Platform not supported
#endif

#include "AL/OS/SystemException.hpp"

#include <pico/multicore.h>

namespace AL::OS::Pico
{
	class Core1
	{
		inline static Bool isRunning = False;

		Core1() = delete;

	public:
		static Bool IsRunning()
		{
			return isRunning;
		}

		template<typename ... TArgs>
		static Bool Start(Void(*lpFunction)(TArgs ...), TArgs ... args)
		{
			if (IsRunning())
			{

				return False;
			}

			::multicore_launch_core1(
				&Core1::Main<TArgs ...>
			);

			Push(
				lpFunction,
				args ...
			);

			return True;
		}

		static Void Reset()
		{
			if (IsRunning())
			{
				::multicore_reset_core1();

				isRunning = False;
			}
		}

		template<typename ... TArgs>
		static Void Push(TArgs ... args)
		{
			PushAll(
				args ...
			);
		}

		template<typename T>
		static T    Pop()
		{
			T value;
			Pop(value);

			return value;
		}
		template<typename ... TArgs>
		static Void Pop(TArgs& ... args)
		{
			PopAll(
				args ...
			);
		}

		template<typename T>
		static Bool TryPop(T& arg, TimeSpan timeout)
		{
			if (!::multicore_fifo_pop_timeout_us(timeout.ToMicroseconds(), reinterpret_cast<uint32_t*>(&arg)))
			{

				return False;
			}

			return True;
		}

	private:
		static constexpr Void PushAll()
		{
		}
		template<typename T, typename ... TArgs>
		static constexpr Void PushAll(T arg, TArgs ... args)
		{
			static_assert(sizeof(T) <= sizeof(::uint32_t));

			if constexpr (Is_Enum_Or_Integer<T>::Value)
				::multicore_fifo_push_blocking(static_cast<::uint32_t>(arg));
			else
				::multicore_fifo_push_blocking(reinterpret_cast<::uint32_t>(arg));

			PushAll(
				args ...
			);
		}

		static constexpr Void PopAll()
		{
		}
		template<typename T, typename ... TArgs>
		static constexpr Void PopAll(T& arg, TArgs& ... args)
		{
			static_assert(sizeof(T) <= sizeof(::uint32_t));

			if constexpr (Is_Enum_Or_Integer<T>::Value)
				arg = static_cast<T>(::multicore_fifo_pop_blocking());
			else
				arg = reinterpret_cast<T>(::multicore_fifo_pop_blocking());

			PopAll(
				args ...
			);
		}

		template<typename ... TArgs>
		static constexpr Void Main()
		{
			isRunning = True;

			Pop<Void(*)(TArgs ...)>()(
				Pop<TArgs>() ...
			);

			isRunning = False;
		}
	};
}
