#pragma once
#include "AL/Common.hpp"

#include "Instruction.hpp"

namespace AL::Assembly::Instructions
{
	class Copy
		: public Instruction
	{
		enum class Types
		{
			CopyAddressToMemory,
			CopyAddressToRegister,

			CopyMemoryToMemory,
			CopyMemoryToRegister,

			CopyRegisterToMemory,
			CopyRegisterToRegister
		};

		union Context
		{
			typename Assembly::Address::Type Address;
			Registers                        Register;
		};

		Buffer buffer;
		
		Copy(Types type, Context&& source, Context&& destination)
		{
			switch (type)
			{
				case Types::CopyAddressToMemory:
				{
#if defined(AL_X86)
					throw Exceptions::NotImplementedException();
#elif defined(AL_X86_64)
					throw Exceptions::NotImplementedException();
#elif defined(AL_ARM) || defined(AL_ARM64)
					throw Exceptions::NotImplementedException();
#endif
				}
				break;

				case Types::CopyAddressToRegister:
				{
#if defined(AL_X86)
					buffer.SetSize(
						sizeof(uint8) + sizeof(typename Address::Type)
					);

					auto lpOPCode = reinterpret_cast<uint8*>(
						&buffer[0]
					);

					switch (destination.Register)
					{
						case Registers::EAX:
							*lpOPCode = 0xB8;
							break;

						case Registers::ECX:
							*lpOPCode = 0xB9;
							break;

						case Registers::EDX:
							*lpOPCode = 0xBA;
							break;

						case Registers::EBX:
							*lpOPCode = 0xBB;
							break;

						case Registers::ESP:
							*lpOPCode = 0xBC;
							break;

						case Registers::EBP:
							*lpOPCode = 0xBD;
							break;

						case Registers::ESI:
							*lpOPCode = 0xBE;
							break;

						case Registers::EDI:
							*lpOPCode = 0xBF;
							break;
					}

					auto lpAddress = reinterpret_cast<typename Address::Type*>(
						&buffer[sizeof(uint8)]
					);

					*lpAddress = source.Address;
#elif defined(AL_X86_64)
					throw Exceptions::NotImplementedException();
#elif defined(AL_ARM) || defined(AL_ARM64)
					throw Exceptions::NotImplementedException();
#endif
				}
				break;

				case Types::CopyMemoryToMemory:
				{
#if defined(AL_X86)
					throw Exceptions::NotImplementedException();
#elif defined(AL_X86_64)
					throw Exceptions::NotImplementedException();
#elif defined(AL_ARM) || defined(AL_ARM64)
					throw Exceptions::NotImplementedException();
#endif
				}
				break;

				case Types::CopyMemoryToRegister:
				{
#if defined(AL_X86)
					throw Exceptions::NotImplementedException();
#elif defined(AL_X86_64)
					throw Exceptions::NotImplementedException();
#elif defined(AL_ARM) || defined(AL_ARM64)
					throw Exceptions::NotImplementedException();
#endif
				}
				break;

				case Types::CopyRegisterToMemory:
				{
#if defined(AL_X86)
					throw Exceptions::NotImplementedException();
#elif defined(AL_X86_64)
					throw Exceptions::NotImplementedException();
#elif defined(AL_ARM) || defined(AL_ARM64)
					throw Exceptions::NotImplementedException();
#endif
				}
				break;

				case Types::CopyRegisterToRegister:
				{
#if defined(AL_X86)
					throw Exceptions::NotImplementedException();
#elif defined(AL_X86_64)
					throw Exceptions::NotImplementedException();
#elif defined(AL_ARM) || defined(AL_ARM64)
					throw Exceptions::NotImplementedException();
#endif
				}
				break;
			}
		}

	public:
		static Copy AddressToMemory(Address source, Address destination)
		{
			Context _source;
			_source.Address = source;

			Context _destination;
			_destination.Address = destination;

			return Copy(
				Types::CopyAddressToMemory,
				AL::Move(_source),
				AL::Move(_destination)
			);
		}
		static Copy AddressToRegister(Address source, Registers destination)
		{
			Context _source;
			_source.Address = source;

			Context _destination;
			_destination.Register = destination;

			return Copy(
				Types::CopyAddressToRegister,
				AL::Move(_source),
				AL::Move(_destination)
			);
		}

		static Copy MemoryToMemory(Address source, Address destination)
		{
			Context _source;
			_source.Address = source;

			Context _destination;
			_destination.Address = destination;

			return Copy(
				Types::CopyMemoryToMemory,
				AL::Move(_source),
				AL::Move(_destination)
			);
		}
		static Copy MemoryToRegister(Address source, Registers destination)
		{
			Context _source;
			_source.Address = source;

			Context _destination;
			_destination.Register = destination;

			return Copy(
				Types::CopyMemoryToRegister,
				AL::Move(_source),
				AL::Move(_destination)
			);
		}

		static Copy RegisterToMemory(Registers source, Address destination)
		{
			Context _source;
			_source.Register = source;

			Context _destination;
			_destination.Address = destination;

			return Copy(
				Types::CopyRegisterToMemory,
				AL::Move(_source),
				AL::Move(_destination)
			);
		}
		static Copy RegisterToRegister(Registers source, Registers destination)
		{
			Context _source;
			_source.Register = source;

			Context _destination;
			_destination.Register = destination;

			return Copy(
				Types::CopyRegisterToRegister,
				AL::Move(_source),
				AL::Move(_destination)
			);
		}

		Copy()
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
