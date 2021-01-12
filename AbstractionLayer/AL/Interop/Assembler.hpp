#pragma once
#include "AL/Common.hpp"

#include "Instructions/Add.hpp"
#include "Instructions/Call.hpp"
#include "Instructions/Compare.hpp"
#include "Instructions/Copy.hpp"
#include "Instructions/Divide.hpp"
#include "Instructions/Jump.hpp"
#include "Instructions/JumpIfEqual.hpp"
#include "Instructions/JumpIfGreaterThan.hpp"
#include "Instructions/JumpIfLessThan.hpp"
#include "Instructions/JumpIfNotEqual.hpp"
#include "Instructions/JumpIfNotZero.hpp"
#include "Instructions/Multiply.hpp"
#include "Instructions/Pop.hpp"
#include "Instructions/Push.hpp"
#include "Instructions/Return.hpp"
#include "Instructions/ShiftLeft.hpp"
#include "Instructions/ShiftRight.hpp"
#include "Instructions/Subtract.hpp"

namespace AL::Interop
{
	class Assembler
	{
		Assembler() = delete;

	public:
		template<typename ... T_INSTRUCTION>
		static bool Assemble(Buffer& buffer, const T_INSTRUCTION& ... instructions)
		{
			size_t bufferSize = 0;

			auto for_each_get_buffer_size = [&bufferSize](const Instructions::Instruction& _instruction)
			{
				bufferSize += _instruction.GetSize();

				return true;
			};

			if (For_Each(for_each_get_buffer_size, instructions ...))
			{
				buffer.SetSize(
					bufferSize
				);

				size_t bufferOffset = 0;

				auto for_each_assemble_instructions = [&buffer, &bufferOffset, &bufferSize](const Instructions::Instruction& _instruction)
				{
					if (!_instruction.Assemble(&buffer[0], bufferOffset, bufferSize))
					{

						return false;
					}
					
					bufferOffset += _instruction.GetSize();

					return true;
				};

				if (For_Each(for_each_assemble_instructions, instructions ...))
				{

					return true;
				}
			}

			return false;
		}

	private:
		template<typename F>
		static bool For_Each(F&& callback)
		{
			return true;
		}
		template<typename F, typename T_INSTRUCTION, typename ... T_INSTRUCTIONS>
		static bool For_Each(F&& callback, const T_INSTRUCTION& instruction, const T_INSTRUCTIONS& ... instructions)
		{
			return callback(instruction) && For_Each(callback, instructions ...);
		}
	};
}
