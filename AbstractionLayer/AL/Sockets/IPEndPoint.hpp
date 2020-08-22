#pragma once
#include "AL/Common.hpp"

#include "IPAddress.hpp"

namespace AL::Sockets
{
	struct IPEndPoint
	{
		IPAddress Address;
		uint16    Port;

		// @throw AL::Exceptions::Exception
		static void FromString(IPEndPoint& ep, const String& string)
		{
			size_t separatorIndex;

			if ((separatorIndex = string.IndexOfLast(':')) == String::NPOS)
			{

				throw Exceptions::Exception(
					"Invalid format: No port specified"
				);
			}

			if (!separatorIndex || (separatorIndex == (string.GetLength() - 1)))
			{

				throw Exceptions::Exception(
					"Invalid format: Separator in wrong location"
				);
			}

			auto addressString = string.SubString(
				0,
				separatorIndex - 1
			);

			IPAddress::FromString(
				ep.Address,
				addressString
			);

			auto portString = string.SubString(
				separatorIndex + 1
			);

			ep.Port = Integer<uint16>::FromString(
				portString
			);
		}

		IPEndPoint()
			: IPEndPoint(
				IPAddress(),
				0
			)
		{
		}

		IPEndPoint(IPAddress&& address, uint16 port)
			: Address(
				Move(address)
			),
			Port(
				port
			)
		{
		}
		
		IPEndPoint(const IPAddress& address, uint16 port)
			: IPEndPoint(
				IPAddress(address),
				port
			)
		{
		}

		bool Cast(IPEndPoint& ep, AddressFamilies family) const
		{
			if (!Address.Cast(ep.Address, family))
			{

				return false;
			}

			ep.Port = Port;

			return true;
		}

		String ToString() const
		{
			return String::Format(
				"%s:%u",
				Address.ToString().GetCString(),
				Port
			);
		}
	};
}
