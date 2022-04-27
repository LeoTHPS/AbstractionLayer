#pragma once
#include <AL/Common.hpp>

#include <AL/OS/Console.hpp>

#include <AL/Hardware/NetworkAdapter.hpp>

// @throw AL::Exception
static void AL_Hardware_NetworkAdapter()
{
	using namespace AL;
	using namespace AL::Hardware;

	NetworkAdapterEnumCallback callback(
		[](const NetworkAdapter& _networkAdapter)
		{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
			OS::Console::WriteLine("Name: %s", _networkAdapter.Name.GetCString());
			OS::Console::WriteLine("Description: %s", _networkAdapter.Description.GetCString());
			OS::Console::WriteLine("MAC Address: %s", _networkAdapter.MAC.GetCString());
			OS::Console::WriteLine("MTU: %lu", _networkAdapter.MTU);
			OS::Console::WriteLine("Type: %u", _networkAdapter.Type);
			OS::Console::WriteLine("Status: %u", _networkAdapter.Status);
			OS::Console::WriteLine("Flags: 0x%04X", _networkAdapter.Flags.Value);
			OS::Console::WriteLine("DHCP IPv4: %s", _networkAdapter.DHCP.Server.IPv4.ToString().GetCString());
			OS::Console::WriteLine("DHCP IPv6: %s", _networkAdapter.DHCP.Server.IPv6.ToString().GetCString());
			for (auto& dns : _networkAdapter.DNS) OS::Console::WriteLine("DNS: %s", dns.ToString().GetCString());
			for (auto& gateway : _networkAdapter.Gateway) OS::Console::WriteLine("Gateway: %s", gateway.ToString().GetCString());
			for (auto& address : _networkAdapter.Addresses) OS::Console::WriteLine("Address: %s", address.ToString().GetCString());
			OS::Console::WriteLine("Link Speed: %llu / %llu", _networkAdapter.LinkSpeed.TX, _networkAdapter.LinkSpeed.RX);
			OS::Console::WriteLine();
#endif

			return True;
		}
	);

	NetworkAdapter::Enumerate(
		callback
	);
}
