#pragma once
#include "AL/Common.hpp"

#include "PacketHeader.hpp"

#include "AL/Algorithms/FNV.hpp"

#include "AL/Collections/ByteBuffer.hpp"

namespace AL::Game::Network::Protocol
{
	template<typename T>
	struct _Packet_Hash;
	template<>
	struct _Packet_Hash<uint32>
	{
		static uint32 Calculate(const Void* lpBuffer, size_t size)
		{
			return Algorithms::FNV32::Calculate(
				lpBuffer,
				size
			);
		}
	};
	template<>
	struct _Packet_Hash<uint64>
	{
		static uint32 Calculate(const Void* lpBuffer, size_t size)
		{
			return Algorithms::FNV64::Calculate(
				lpBuffer,
				size
			);
		}
	};

	template<typename T_OPCODE>
	using Packet_Hash = _Packet_Hash<decltype(PacketHeader<T_OPCODE>::DataHash)>;

	template<typename T_OPCODE>
	class Packet
	{
		typedef T_OPCODE                              _OPCode;
		typedef Packet_Hash<_OPCode>                  _Hash;
		typedef Collections::ByteBuffer<Endians::Big> _Buffer;
		typedef Protocol::PacketHeader<_OPCode>       _Header;

		Bool    isFinalized = False;

		_Header header;
		_Buffer buffer;

	public:
		typedef _OPCode OPCode;

		template<typename T>
		static constexpr T ToEndian(T value)
		{
			return _Buffer::ToEndian(
				Forward<T>(value)
			);
		}

		template<typename T>
		static constexpr T FromEndian(T value)
		{
			return _Buffer::FromEndian(
				Forward<T>(value)
			);
		}

		// @throw AL::Exception
		static Packet FromBuffer(const Void* lpBuffer, size_t size)
		{
			if (size < sizeof(_Header))
			{

				throw Exception(
					"Unexpected end of buffer"
				);
			}

			Packet packet;

			AL::memcpy(
				&packet.header,
				lpBuffer,
				sizeof(_Header)
			);

			size    -= sizeof(_Header);
			lpBuffer = &reinterpret_cast<const uint8*>(lpBuffer)[sizeof(_Header)];

			packet.header.OPCode   = FromEndian(packet.header.OPCode);
			packet.header.DataSize = FromEndian(packet.header.DataSize);
			packet.header.DataHash = FromEndian(packet.header.DataHash);

			if (packet.header.DataSize != size)
			{

				throw Exception(
					"Invalid data size"
				);
			}

			if (packet.header.DataHash != _Hash::Calculate(lpBuffer, size))
			{

				throw Exception(
					"Invalid data hash"
				);
			}

			packet.isFinalized = True;

			// This only fails if the buffer isn't self hosted, which it is
			packet.buffer.SetCapacity(sizeof(_Header) + packet.header.DataSize, False);

			packet.buffer.Write(packet.header.OPCode);
			packet.buffer.Write(packet.header.DataSize);
			packet.buffer.Write(packet.header.DataHash);
			packet.buffer.Write(lpBuffer, size);

			packet.buffer.SetReadPosition(
				sizeof(_Header)
			);

			return packet;
		}

		Packet()
			: buffer(
				sizeof(_Header)
			)
		{
		}

		Packet(Packet&& packet)
			: isFinalized(
				packet.isFinalized
			),
			header(
				Move(packet.header)
			),
			buffer(
				Move(packet.buffer)
			)
		{
			packet.isFinalized = False;
		}

		Packet(const Packet& packet)
			: isFinalized(
				packet.isFinalized
			),
			header(
				packet.header
			),
			buffer(
				packet.buffer
			)
		{
		}

		Packet(OPCode opcode, uint16 capacity)
			: header(
				_Header
				{
					.OPCode   = opcode,
					.DataSize = 0,
					.DataHash = 0
				}
			),
			buffer(
				sizeof(_Header) + capacity
			)
		{
			buffer.SetWritePosition(
				sizeof(_Header)
			);

			buffer.SetReadPosition(
				sizeof(_Header)
			);
		}

		virtual ~Packet()
		{
		}

		Bool IsFinalized() const
		{
			return isFinalized;
		}

		auto GetSize() const
		{
			return header.DataSize;
		}

		auto GetBuffer() const
		{
			return buffer.GetBuffer();
		}

		auto GetBufferSize() const
		{
			return GetSize() + sizeof(_Header);
		}

		auto GetOPCode() const
		{
			return header.OPCode;
		}

		auto GetLastReadSize() const
		{
			return buffer.GetLastReadSize();
		}

		auto GetLastWriteSize() const
		{
			return buffer.GetLastWriteSize();
		}

		template<typename T>
		inline Bool Read(T& value)
		{
			if (!buffer.Read(value))
			{

				return False;
			}

			return True;
		}
		inline Bool Read(String& value)
		{
			if (!buffer.ReadString(value))
			{

				return False;
			}

			return True;
		}

		template<typename T>
		inline Bool Write(T value)
		{
			if (!buffer.Write(value))
			{

				return False;
			}

			isFinalized = False;

			header.DataSize += GetLastWriteSize();

			return True;
		}
		inline Bool Write(const String& value)
		{
			if (!buffer.WriteString(value))
			{

				return False;
			}

			isFinalized = False;

			header.DataSize += GetLastWriteSize();

			return True;
		}

		Void Finalize()
		{
			header.DataHash = _Hash::Calculate(
				&reinterpret_cast<const uint8*>(GetBuffer())[sizeof(_Header)],
				GetSize()
			);

			auto writePosition = buffer.GetWritePosition();

			buffer.SetWritePosition(
				0
			);

			buffer.Write(header.OPCode);
			buffer.Write(header.DataSize);
			buffer.Write(header.DataHash);

			buffer.SetWritePosition(
				writePosition
			);

			isFinalized = True;
		}

		Packet& operator = (Packet&& packet)
		{
			isFinalized = packet.isFinalized;
			packet.isFinalized = False;

			header = Move(
				packet.header
			);

			buffer = Move(
				packet.buffer
			);

			return *this;
		}
		Packet& operator = (const Packet& packet)
		{
			isFinalized = packet.isFinalized;

			header      = packet.header;
			buffer      = packet.buffer;

			return *this;
		}

		Bool operator == (const Packet& packet) const
		{
			if (GetOPCode() != packet.GetOPCode())
			{

				return False;
			}

			size_t size;

			if ((size = GetSize()) != packet.GetSize())
			{

				return False;
			}

			if (!memcmp(&reinterpret_cast<const uint8*>(GetBuffer())[sizeof(_Header)], &reinterpret_cast<const uint8*>(packet.GetBuffer())[sizeof(_Header)], size))
			{

				return False;
			}

			return True;
		}
		Bool operator != (const Packet& packet) const
		{
			if (operator==(packet))
			{

				return False;
			}

			return True;
		}
	};
}
