#pragma once
#include "AL/Common.hpp"

#include "IDriver.hpp"

#include "AL/Hardware/I2C.hpp"

#include "AL/Collections/Array.hpp"

namespace AL::Hardware::Drivers
{
	class AT24C256
		: public IDriver<Void, Void, Void>
	{
		Bool      isOpen = False;
		Bool      isBusAllocated = False;

		I2CBus*   lpBus;
		I2CDevice device;

	public:
		static constexpr uint16  PAGE_SIZE       = 0x40;

		inline static I2CAddress DEVICE_ADDRESS  = 0x50;

		static constexpr uint16  ADDRESS_MINIMUM = 0x0000;
		static constexpr uint16  ADDRESS_MAXIMUM = 0x8000;

		AT24C256(AT24C256&& at24c256)
			: isOpen(
				at24c256.isOpen
			),
			isBusAllocated(
				at24c256.isBusAllocated
			),
			lpBus(
				at24c256.lpBus
			),
			device(
				Move(at24c256.device)
			)
		{
			at24c256.isOpen = False;
			at24c256.isBusAllocated = False;
		}

		AT24C256(I2CBus& bus, I2CAddress address = DEVICE_ADDRESS)
			: isBusAllocated(
				False
			),
			lpBus(
				&bus
			),
			device(
				bus,
				address
			)
		{
		}

		AT24C256(FileSystem::Path&& path, I2CAddress address = DEVICE_ADDRESS)
			: isBusAllocated(
				True
			),
			lpBus(
				new I2CBus(
					Move(path)
				)
			),
			device(
				*lpBus,
				address
			)
		{
		}
		AT24C256(const FileSystem::Path& path, I2CAddress address = DEVICE_ADDRESS)
			: AT24C256(
				FileSystem::Path(path),
				address
			)
		{
		}

		virtual ~AT24C256()
		{
			if (IsOpen())
			{

				Close();
			}

			if (isBusAllocated)
			{
				delete lpBus;

				isBusAllocated = False;
			}
		}

		virtual Bool IsOpen() const override
		{
			return isOpen;
		}

		// @throw AL::Exception
		virtual Void Open() override
		{
			AL_ASSERT(
				!IsOpen(),
				"AT24C256 already open"
			);

			if (isBusAllocated)
			{
				try
				{
					lpBus->Open();
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error opening bus"
					);
				}
			}

			isOpen = True;
		}

		virtual Void Close() override
		{
			if (IsOpen())
			{
				if (isBusAllocated)
				{

					lpBus->Close();
				}

				isOpen = False;
			}
		}

		// @throw AL::Exception
		Void Clear()
		{
			AL_ASSERT(
				IsOpen(),
				"AT24C256 not open"
			);

			Clear(
				ADDRESS_MINIMUM,
				ADDRESS_MAXIMUM - ADDRESS_MINIMUM
			);
		}
		// @throw AL::Exception
		Void Clear(uint16 address, size_t count)
		{
			for (size_t totalBytesCleared = 0; totalBytesCleared < count; )
			{
				auto numberOfBytesCleared = ClearPage(
					address,
					count - totalBytesCleared
				);

				Spin(
					TimeSpan::FromMilliseconds(10)
				);

				address           += numberOfBytesCleared;
				totalBytesCleared += numberOfBytesCleared;
			}
		}

		// @throw AL::Exception
		template<typename T>
		Bool Read(uint16 address, T& value)
		{
			AL_ASSERT(
				IsOpen(),
				"AT24C256 not open"
			);

			if (GetNumberOfBytesRemainingAtAddress(address) < sizeof(T))
			{

				return False;
			}

			for (size_t totalBytesRead = 0; totalBytesRead < sizeof(T); )
			{
				auto numberOfBytesRead = ReadPage(
					address + totalBytesRead,
					&reinterpret_cast<uint8*>(&value)[totalBytesRead],
					sizeof(T) - totalBytesRead
				);

				totalBytesRead += numberOfBytesRead;
			}

			return True;
		}

		// @throw AL::Exception
		template<typename T>
		Bool Write(uint16 address, const T& value)
		{
			AL_ASSERT(
				IsOpen(),
				"AT24C256 not open"
			);

			if (GetNumberOfBytesRemainingAtAddress(address) < sizeof(T))
			{

				return False;
			}

			for (size_t totalBytesWritten = 0; totalBytesWritten < sizeof(T); )
			{
				auto numberOfBytesWritten = WritePage(
					address + totalBytesWritten,
					&reinterpret_cast<const uint8*>(&value)[totalBytesWritten],
					sizeof(T) - totalBytesWritten
				);

				Spin(
					TimeSpan::FromMilliseconds(10)
				);

				totalBytesWritten += numberOfBytesWritten;
			}

			return True;
		}

		AT24C256& operator = (AT24C256&& at24c256)
		{
			Close();

			isOpen = at24c256.isOpen;
			at24c256.isOpen = False;

			isBusAllocated = at24c256.isBusAllocated;
			at24c256.isBusAllocated = False;

			lpBus = at24c256.lpBus;

			device = Move(
				at24c256.device
			);

			return *this;
		}

		Bool operator == (const AT24C256& at24c256) const
		{
			if (device != at24c256.device)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const AT24C256& at24c256) const
		{
			if (operator==(at24c256))
			{

				return False;
			}

			return True;
		}

	private:
		// @throw AL::Exception
		// @return number of bytes cleared
		size_t ClearPage(uint16 address, size_t count)
		{
			if (count > PAGE_SIZE)
			{

				count = PAGE_SIZE;
			}

			Collections::Array<uint8> buffer(
				2 + count
			);

			memset(&buffer[2], 0, PAGE_SIZE);
			buffer[0] = static_cast<uint8>(address >> 8);
			buffer[1] = static_cast<uint8>(address & 0x00FF);

			device.Write(
				&buffer[0],
				buffer.GetCapacity()
			);

			return count;
		}

		// @throw AL::Exception
		// @return number of bytes read
		size_t ReadPage(uint16 address, uint8* lpValues, size_t count)
		{
			if (count > PAGE_SIZE)
			{

				count = PAGE_SIZE;
			}

			// Address
			{
				uint8 buffer[] =
				{
					static_cast<uint8>(address >> 8),
					static_cast<uint8>(address & 0x00FF)
				};

				device.Write(
					buffer
				);
			}

			device.Read(
				lpValues,
				count
			);

			return count;
		}

		// @throw AL::Exception
		// @return number of bytes written
		size_t WritePage(uint16 address, const uint8* lpValues, size_t count)
		{
			if (count > PAGE_SIZE)
			{

				count = PAGE_SIZE;
			}

			Collections::Array<uint8> buffer(
				sizeof(uint16) + count
			);

			memcpy(&buffer[2], lpValues, count);
			buffer[0] = static_cast<uint8>(address >> 8);
			buffer[1] = static_cast<uint8>(address & 0x00FF);

			device.Write(
				&buffer[0],
				buffer.GetCapacity()
			);

			return count;
		}

		static size_t GetNumberOfBytesRemainingAtAddress(uint16 address)
		{
			if (address >= ADDRESS_MAXIMUM)
			{

				return 0;
			}

			return ADDRESS_MAXIMUM - address;
		}
	};
}
