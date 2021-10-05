#pragma once
#include "AL/Common.hpp"

#include "Packet.hpp"

#include "AL/Collections/Array.hpp"
#include "AL/Collections/Queue.hpp"

namespace AL::Game::Network::Protocol
{
	template<typename T_OPCODE, Endians ENDIAN>
	class PacketBuilder
	{
		typedef Protocol::Packet<T_OPCODE, ENDIAN> _Packet;

		Collections::Array<uint8>   buffer;
		size_t                      bufferSize = 0;

		Collections::Queue<_Packet> packets;

	public:
		typedef _Packet Packet;

		explicit PacketBuilder(size_t capacity)
			: buffer(
				capacity
			)
		{
		}

		PacketBuilder(PacketBuilder&& packetBuilder)
			: buffer(
				Move(packetBuilder.buffer)
			),
			bufferSize(
				packetBuilder.bufferSize
			),
			packets(
				Move(packetBuilder.packets)
			)
		{
		}
		PacketBuilder(const PacketBuilder& packetBuilder)
			: buffer(
				packetBuilder.bufferSize
			),
			bufferSize(
				packetBuilder.bufferSize
			),
			packets(
				packetBuilder.packets
			)
		{
		}

		virtual ~PacketBuilder()
		{
		}

		// Packet count
		auto GetSize() const
		{
			return packets.GetSize();
		}

		// Buffer size in bytes
		auto GetCapacity() const
		{
			return buffer.GetCapacity();
		}

		Void Clear()
		{
			bufferSize = 0;

			packets.Clear();
		}

		Bool Pop(Packet& packet)
		{
			if (!packets.Dequeue(packet))
			{

				return False;
			}

			return True;
		}

		// @throw AL::Exception
		// @return number of packets completed
		size_t Push(const Void* lpBuffer, size_t size);

		PacketBuilder& operator = (PacketBuilder&& packetBuilder)
		{
			buffer = Move(
				packetBuilder.buffer
			);

			bufferSize = packetBuilder.bufferSize;
			packetBuilder.bufferSize = 0;

			packets = Move(
				packetBuilder.packets
			);

			return *this;
		}
		PacketBuilder& operator = (const PacketBuilder& packetBuilder)
		{
			buffer     = packetBuilder.buffer;
			bufferSize = packetBuilder.bufferSize;

			packets    = packetBuilder.packets;

			return *this;
		}

		Bool operator == (const PacketBuilder& packetBuilder) const
		{
			if (bufferSize != packetBuilder.bufferSize)
			{

				return False;
			}

			if (buffer.GetCapacity() != packetBuilder.buffer.GetCapacity())
			{

				return False;
			}

			if (!memcmp(&buffer[0], &packetBuilder.buffer[0], bufferSize))
			{

				return False;
			}

			if (packets != packetBuilder.packets)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const PacketBuilder& packetBuilder) const
		{
			if (operator==(packetBuilder))
			{

				return False;
			}

			return True;
		}
	};
}
