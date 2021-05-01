#pragma once
#include "AL/Common.hpp"

namespace AL::Assembly
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

		template<typename T>
		Address(T* value)
			: Address(
				reinterpret_cast<Type>(
					value
				)
			)
		{
		}

		Address(Type value)
			: value(
				value
			)
		{
		}

		Address(std::nullptr_t)
			: Address(
				Type(0)
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
