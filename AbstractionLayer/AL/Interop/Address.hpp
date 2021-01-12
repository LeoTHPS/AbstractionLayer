#pragma once
#include "AL/Common.hpp"

namespace AL::Interop
{
	class Address
	{
	public:
#if defined(AL_X86) || defined(AL_ARM)
		typedef uint32 Type;
#elif defined(AL_X86_64) || defined(AL_ARM64)
		typedef uint64 Type;
#endif

		Address()
			: Address(
				nullptr
			)
		{
		}

		Address(Type value)
			: value(
				value
			)
		{
		}

		Address(void* value)
			: Address(
				reinterpret_cast<Type>(
					value
				)
			)
		{
		}

		virtual ~Address()
		{
		}

		operator Type&()
		{
			return value;
		}
		operator Type() const
		{
			return value;
		}

	private:
		Type value;
	};
}
