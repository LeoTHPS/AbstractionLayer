#pragma once
#include <AL/Common.hpp>

#include <AL/OS/Console.hpp>

#include <AL/Network/Adapter.hpp>

// @throw AL::Exception
static void AL_Network_Adapter()
{
	using namespace AL;
	using namespace AL::Network;

	AdapterEnumCallback callback(
		[](const Adapter& _adapter)
		{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
			OS::Console::WriteLine("Name: %s", _adapter.Name.GetCString());
			OS::Console::WriteLine("Description: %s", _adapter.Description.GetCString());
			OS::Console::WriteLine("MAC Address: %s", _adapter.MAC.GetCString());
			OS::Console::WriteLine("MTU: %lu", _adapter.MTU);
			OS::Console::WriteLine("Type: %u", _adapter.Type);
			OS::Console::WriteLine("Status: %u", _adapter.Status);
			OS::Console::WriteLine("Flags: 0x%04X", _adapter.Flags.Value);
			OS::Console::WriteLine("DHCP IPv4: %s", _adapter.DHCP.Server.IPv4.ToString().GetCString());
			OS::Console::WriteLine("DHCP IPv6: %s", _adapter.DHCP.Server.IPv6.ToString().GetCString());
			for (auto& dns : _adapter.DNS) OS::Console::WriteLine("DNS: %s", dns.ToString().GetCString());
			for (auto& gateway : _adapter.Gateway) OS::Console::WriteLine("Gateway: %s", gateway.ToString().GetCString());
			for (auto& address : _adapter.Addresses) OS::Console::WriteLine("Address: %s", address.ToString().GetCString());
			OS::Console::WriteLine("Link Speed: %llu / %llu", _adapter.LinkSpeed.TX, _adapter.LinkSpeed.RX);
			OS::Console::WriteLine();
#endif

			return True;
		}
	);

	Adapter::Enumerate(
		callback
	);
}
