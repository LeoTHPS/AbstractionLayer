#pragma once
#include "AL/Common.hpp"

#include "Instruction.hpp"

namespace AL::Assembly::Instructions
{
	class Call
		: public Instruction
	{
		enum class Types
		{
			CallMemory,
			CallAddress,
			CallRegister
		};

		Buffer buffer;
		
		explicit Call(Registers _register)
		{
			BuildBuffer(
				Types::CallRegister,
				Assembly::Address::Type(0),
				Assembly::Address::Type(0),
				_register
			);
		}
		
		Call(Assembly::Address address, bool dereference)
		{
			BuildBuffer(
				dereference ? Types::CallMemory : Types::CallAddress,
				dereference ? address : nullptr,
				!dereference ? address : nullptr,
				Registers(0)
			);
		}

	public:
		static Call Memory(Assembly::Address address)
		{
			return Call(
				address,
				true
			);
		}

		static Call Address(Assembly::Address address)
		{
			return Call(
				address,
				false
			);
		}

		static Call Register(Registers _register)
		{
			return Call(
				_register
			);
		}

		Call()
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

	private:
		void BuildBuffer(Types type, Assembly::Address memory, Assembly::Address address, Registers _register)
		{
			switch (type)
			{
				case Types::CallMemory:
				{
#if defined(AL_X86)
					buffer.SetSize(
						sizeof(uint16) + sizeof(uint32)
					);

					auto lpOPCode = reinterpret_cast<uint16*>(
						&buffer[0]
					);
					
					auto lpAddress = reinterpret_cast<uint32*>(
						&buffer[sizeof(uint16)]
					);

					*lpOPCode = 0xFF15;
					*lpAddress = address;
#elif defined(AL_X86_64)
					throw Exceptions::NotImplementedException();
#elif defined(AL_ARM) || defined(AL_ARM64)
					throw Exceptions::NotImplementedException();
#endif
				}
				break;

				case Types::CallAddress:
				{
#if defined(AL_X86)
					buffer.SetSize(
						sizeof(uint8) + sizeof(uint32)
					);

					auto lpOPCode = reinterpret_cast<uint8*>(
						&buffer[0]
					);

					auto lpAddress = reinterpret_cast<uint32*>(
						&buffer[sizeof(uint8)]
					);

					*lpOPCode = 0xE8;
					*lpAddress = address;
#elif defined(AL_X86_64)
					throw Exceptions::NotImplementedException();
#elif defined(AL_ARM) || defined(AL_ARM64)
					throw Exceptions::NotImplementedException();
#endif
				}
				break;

				case Types::CallRegister:
				{
#if defined(AL_X86)
					buffer.SetSize(
						sizeof(uint8) + sizeof(uint8)
					);

					auto lpOPCode = reinterpret_cast<uint8*>(
						&buffer[0]
					);

					*lpOPCode = 0xFF;

					auto lpRegister = reinterpret_cast<uint8*>(
						&buffer[sizeof(uint8)]
					);

					switch (_register)
					{
						case Registers::EAX:
							*lpRegister = 0xD0;
							break;

						case Registers::ECX:
							*lpRegister = 0xD1;
							break;

						case Registers::EDX:
							*lpRegister = 0xD2;
							break;

						case Registers::EBX:
							*lpRegister = 0xD3;
							break;

						case Registers::ESP:
							*lpRegister = 0xD4;
							break;

						case Registers::EBP:
							*lpRegister = 0xD5;
							break;

						case Registers::ESI:
							*lpRegister = 0xD6;
							break;

						case Registers::EDI:
							*lpRegister = 0xD7;
							break;
					}
#elif defined(AL_X86_64)
					throw Exceptions::NotImplementedException();
#elif defined(AL_ARM) || defined(AL_ARM64)
					throw Exceptions::NotImplementedException();
#endif
				}
				break;
			}
		}
	};
}
