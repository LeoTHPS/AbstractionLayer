#pragma once
#include <AL/Common.hpp>

#include <AL/OS/System.hpp>
#include <AL/OS/Console.hpp>

#include <AL/Hardware/Drivers/AT24C256.hpp>

static void AL_Hardware_Drivers_AT24C256()
{
	using namespace AL;
	using namespace AL::Hardware;
	using namespace AL::Hardware::Drivers;

	AT24C256 at24c256(
		FileSystem::Path("/dev/i2c-1")
	);

	try
	{
		at24c256.Open();
	}
	catch (Exception& exception)
	{

		throw Exception(
			Move(exception),
			"Error opening AT24C256"
		);
	}

	try
	{
		at24c256.Clear();
	}
	catch (Exception& exception)
	{
		at24c256.Close();

		throw Exception(
			Move(exception),
			"Error clearing AT24C256"
		);
	}

	try
	{
		at24c256.Write(
			AT24C256::ADDRESS_MAXIMUM - AT24C256::PAGE_SIZE,
			OS::System::GetTimestamp().ToSeconds()
		);
	}
	catch (Exception& exception)
	{
		at24c256.Close();

		throw Exception(
			Move(exception),
			"Error writing AT24C256"
		);
	}

	uint8 at24c256_Page[AT24C256::PAGE_SIZE];

	for (uint16 address = AT24C256::ADDRESS_MINIMUM; address < AT24C256::ADDRESS_MAXIMUM; address += AT24C256::PAGE_SIZE)
	{
		try
		{
			if (!at24c256.Read(address, at24c256_Page))
			{

				break;
			}
		}
		catch (Exception& exception)
		{
			at24c256.Close();

			throw Exception(
				Move(exception),
				"Error reading AT24C256 [Address: 0x%04X]",
				address
			);
		}

#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
		OS::Console::WriteLine(
			"[%04X] %s",
			address,
			HexConverter::Encode(at24c256_Page).GetCString()
		);
#endif
	}

	at24c256.Close();
}
