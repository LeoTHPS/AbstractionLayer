#pragma once
#include "AL/Common.hpp"

#include "Instruction.hpp"

namespace AL::Interop::Instructions
{
	class Compare
		: public Instruction
	{
		Buffer buffer;

	public:
		Compare()
		{
		}

		virtual size_t GetSize() const override
		{
			return buffer.GetSize();
		}

		virtual bool Assemble(uint8* lpBuffer, size_t offset, size_t size) const override
		{
			return CopyBuffer(
				buffer,
				lpBuffer,
				offset,
				size
			);
		}
	};
}
