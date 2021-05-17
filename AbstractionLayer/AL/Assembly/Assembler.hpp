#pragma once
#include "AL/Common.hpp"

#include "Buffer.hpp"
#include "Address.hpp"

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
#include "Instructions/Move.hpp"
#include "Instructions/Multiply.hpp"
#include "Instructions/Pop.hpp"
#include "Instructions/Push.hpp"
#include "Instructions/Return.hpp"
#include "Instructions/ShiftLeft.hpp"
#include "Instructions/ShiftRight.hpp"
#include "Instructions/Subtract.hpp"

namespace AL::Assembly
{
	class Assembler
	{
		Assembler() = delete;

	public:
		template<typename ... T_INSTRUCTION>
		static Buffer Assemble(const T_INSTRUCTION& ... instructions)
		{
			size_t bufferSize = 0;

			For_Each(
				[&bufferSize](const Instructions::Instruction& _instruction)
				{
					bufferSize += _instruction.GetSize();
				},
				instructions ...
			);

			Buffer buffer(
				bufferSize
			);

			size_t bufferOffset = 0;

			For_Each(
				[&buffer, &bufferOffset, &bufferSize](const Instructions::Instruction& _instruction)
				{
					_instruction.Assemble(
						&buffer[0],
						bufferOffset,
						bufferSize
					);

					bufferOffset += _instruction.GetSize();
				},
				instructions ...
			);

			return buffer;
		}

	private:
		template<typename F>
		static void For_Each(F&& callback)
		{
		}
		template<typename F, typename T_INSTRUCTION, typename ... T_INSTRUCTIONS>
		static void For_Each(F&& callback, const T_INSTRUCTION& instruction, const T_INSTRUCTIONS& ... instructions)
		{
			callback(instruction);

			For_Each(callback, instructions ...);
		}
	};
}
