#pragma once
#include "AL/Common.hpp"

#include "Instruction.hpp"

namespace AL::Assembly::Instructions
{
	class Jump
		: public Instruction
	{
		Buffer buffer;

	public:
		Jump()
		{
		}

		virtual size_t GetSize() const override
		{
			return buffer.GetSize();
		}

		// @throw AL::Exceptions::Exception
		virtual void Assemble(uint8* lpBuffer, size_t offset, size_t size) const override
		{
			CopyBuffer(
				buffer,
				lpBuffer,
				offset,
				size
			);
		}
	};
}
