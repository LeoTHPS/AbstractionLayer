#pragma once
#include <AL/Common.hpp>

#include <AL/OS/System.hpp>
#include <AL/OS/Console.hpp>

#include <AL/Collections/Array.hpp>

#include <AL/Hardware/Drivers/AT24C256.hpp>

// @throw AL::Exception
static void AL_Hardware_Drivers_AT24C256()
{
	using namespace AL;
	using namespace AL::Hardware;
	using namespace AL::Hardware::Drivers;

	AT24C256 at24c256(
		FileSystem::Path("/dev/i2c-1"),
		100000
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

	// clear
	{
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
	}

	// write
	{
		try
		{
			at24c256.Write(
				0x0000,
				OS::System::GetTimezone()
			);

			at24c256.Write(
				0x0001,
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
	}

	// read all pages
	{
		Collections::Array<uint8> at24c256_Buffer(
			AT24C256::PAGE_COUNT * AT24C256::PAGE_SIZE
		);

		try
		{
			at24c256.Read(
				0x0000,
				&at24c256_Buffer[0],
				at24c256_Buffer.GetCapacity()
			);
		}
		catch (Exception& exception)
		{

			throw Exception(
				Move(exception),
				"Error reading AT24C256"
			);
		}

#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
		OS::Console::WriteLine(
			"%-4s %-7s %s",
			"Page",
			"Address",
			"Buffer"
		);
#endif

		for (AT24C256::Address address = 0; address < at24c256_Buffer.GetCapacity(); address += AT24C256::PAGE_SIZE)
		{
#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
			OS::Console::WriteLine(
				"%-4s %-7s %s",
				ToString(
					address / AT24C256::PAGE_SIZE
				).GetCString(),
				String::Format(
					"%04X",
					address
				).GetCString(),
				HexConverter::Encode(
					&at24c256_Buffer[address],
					AT24C256::PAGE_SIZE
				).GetCString()
			);
#endif
		}
	}

	at24c256.Close();
}
