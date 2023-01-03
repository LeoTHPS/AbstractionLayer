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
		Bool                            isOpen = False;
		Bool                            isBusAllocated = False;

		I2CBus*                         lpBus;
		mutable I2CDevice               device;
		Collections::Array<uint8[0x42]> ioBuffer; // sizeof(Address) + PAGE_SIZE

	public:
		typedef uint16 Address;

		static constexpr size_t        PAGE_SIZE      = 0x40;
		static constexpr size_t        PAGE_COUNT     = 0x200;

		inline static const I2CAddress DEVICE_ADDRESS = 0x50;

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

#if defined(AL_PLATFORM_PICO)
		AT24C256(::i2c_inst* i2c, GPIOPin scl, GPIOPin sda, uint32 baud, I2CAddress address = DEVICE_ADDRESS)
			: isBusAllocated(
				True
			),
			lpBus(
				new I2CBus(
					i2c,
					scl,
					sda,
					baud
				)
			),
			device(
				*lpBus,
				address
			)
		{
		}
#elif defined(AL_PLATFORM_LINUX)
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
#endif

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
						"Error opening I2CBus"
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
		Bool Clear()
		{
			AL_ASSERT(
				IsOpen(),
				"AT24C256 not open"
			);

			if (!Clear(0x0000, PAGE_COUNT * PAGE_SIZE))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		Bool Clear(Address address, size_t count)
		{
			AL_ASSERT(
				IsOpen(),
				"AT24C256 not open"
			);

			if ((address + count) > (PAGE_COUNT * PAGE_SIZE))
			{

				return False;
			}

			memset(
				&ioBuffer[2],
				0,
				PAGE_SIZE
			);

			for (size_t i = 0; i < count; i += PAGE_SIZE, address += PAGE_SIZE)
			{
				ioBuffer[0] = static_cast<uint8>(address >> 8);
				ioBuffer[1] = static_cast<uint8>(address & 0x00FF);

				device.Write(
					&ioBuffer[0],
					sizeof(Address) + (((count - i) > PAGE_SIZE) ? PAGE_SIZE : (count - i))
				);

				Spin( // TODO: replace with polling
					TimeSpan::FromMilliseconds(10)
				);
			}

			return True;
		}

		// @throw AL::Exception
		template<typename T>
		Bool Read(Address address, T& value) const
		{
			AL_ASSERT(
				IsOpen(),
				"AT24C256 not open"
			);

			if (!Read(address, &value, sizeof(T)))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		Bool Read(Address address, Void* lpBuffer, size_t size) const
		{
			AL_ASSERT(
				IsOpen(),
				"AT24C256 not open"
			);

			if ((address + size) > (PAGE_COUNT * PAGE_SIZE))
			{

				return False;
			}

			auto lpByteBuffer = reinterpret_cast<uint8*>(
				lpBuffer
			);

			for (size_t i = 0; i < size; i += PAGE_SIZE, address += PAGE_SIZE, lpByteBuffer += PAGE_SIZE)
			{
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
					lpByteBuffer,
					((size - i) > PAGE_SIZE) ? PAGE_SIZE : (size - i)
				);
			}

			return True;
		}

		// @throw AL::Exception
		template<typename T>
		Bool Write(Address address, const T& value)
		{
			AL_ASSERT(
				IsOpen(),
				"AT24C256 not open"
			);

			if (!Write(address, &value, sizeof(T)))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		Bool Write(Address address, const Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"AT24C256 not open"
			);

			if ((address + size) > (PAGE_COUNT * PAGE_SIZE))
			{

				return False;
			}

			auto lpByteBuffer = reinterpret_cast<const uint8*>(
				lpBuffer
			);

			for (size_t i = 0; i < size; i += PAGE_SIZE, address += PAGE_SIZE, lpByteBuffer += PAGE_SIZE)
			{
				auto byteBufferChunkSize = ((size - i) > PAGE_SIZE) ? PAGE_SIZE : (size - i);

				ioBuffer[0] = static_cast<uint8>(address >> 8);
				ioBuffer[1] = static_cast<uint8>(address & 0x00FF);
				memcpy(&ioBuffer[2], lpByteBuffer, byteBufferChunkSize);

				device.Write(
					&ioBuffer[0],
					sizeof(Address) + byteBufferChunkSize
				);

				Spin( // TODO: replace with polling
					TimeSpan::FromMilliseconds(10)
				);
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
	};
}
