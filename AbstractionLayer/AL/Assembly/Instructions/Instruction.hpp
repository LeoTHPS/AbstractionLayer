#pragma once
#include "AL/Common.hpp"

#include "Registers.hpp"

#include "AL/Assembly/Buffer.hpp"
#include "AL/Assembly/Address.hpp"

namespace AL::Assembly::Instructions
{
	class Instruction
	{
	public:
		Instruction()
		{
		}

		virtual ~Instruction()
		{
		}

		virtual size_t GetSize() const = 0;

		// @throw AL::Exceptions::Exception
		virtual void Assemble(uint8* lpBuffer, size_t offset, size_t size) const = 0;

	protected:
		// @throw AL::Exceptions::Exception
		static void CopyBuffer(const Buffer& source, uint8* lpDestination, size_t offset, size_t size)
		{
			if ((offset >= size) || ((offset + source.GetSize()) > size))
			{

				throw Exceptions::Exception(
					"Buffer overflow"
				);
			}

			memcpy(
				&lpDestination[offset],
				&source[0],
				source.GetSize()
			);
		}
	};
}
