#pragma once
#include "AL/Common.hpp"

#include "Registers.hpp"

#include "AL/Interop/Buffer.hpp"
#include "AL/Interop/Address.hpp"

namespace AL::Interop::Instructions
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

		virtual bool Assemble(uint8* lpBuffer, size_t offset, size_t size) const = 0;

	protected:
		static bool CopyBuffer(const Buffer& source, uint8* lpDestination, size_t offset, size_t size)
		{
			if ((offset >= size) || ((offset + source.GetSize()) > size))
			{

				return false;
			}

			memcpy(
				&lpDestination[offset],
				&source[0],
				source.GetSize()
			);

			return true;
		}
	};
}
