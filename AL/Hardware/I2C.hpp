#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/Array.hpp"

#include "AL/OS/SystemException.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <fcntl.h>
	#include <unistd.h>
	#include <termios.h>

	#include <sys/ioctl.h>

	#if __has_include(<linux/i2c-dev.h>)
		#define AL_DEPENDENCY_LIBI2C_DEV

		extern "C"
		{
			#include <linux/i2c.h>
			#include <linux/i2c-dev.h>
		}
	#endif
#endif

namespace AL::Hardware
{
	class I2CAddress
	{
	public:
		typedef uint16 Type;

		I2CAddress(Type value)
			: value(
				value
			)
		{
			AL_ASSERT(
				!IsReserved(value),
				"I2CAddress is reserved"
			);
		}

		virtual ~I2CAddress()
		{
		}

		operator Type () const
		{
			return value;
		}

		I2CAddress& operator = (Type value)
		{
			AL_ASSERT(
				!IsReserved(value),
				"I2CAddress is reserved"
			);

			this->value = value;

			return *this;
		}

		Bool operator == (const I2CAddress& address) const
		{
			if (value != address.value)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const I2CAddress& address) const
		{
			if (operator==(address))
			{

				return False;
			}

			return True;
		}

	private:
		Type value;

		static constexpr Bool IsReserved(Type value)
		{
			return (value >= 0x08) && (value <= 0x77);
		}
	};

	enum class I2CTransactionFlags : uint16
	{
		Read      = 0x1,
		Stop      = 0x2,
		TenBit    = 0x4,
		NoStart   = 0x8,
		NoReadACK = 0x10,
		IgnoreNAK = 0x20
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(I2CTransactionFlags);

	struct I2CTransaction
	{
		BitMask<I2CTransactionFlags> Flags;
		I2CAddress                   Address;
		Void*                        lpBuffer;
		uint16                       BufferSize;
	};

	class I2CBus
	{
		Bool   isOpen = False;

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBI2C_DEV)
		int    fd;
	#endif
#endif

		String path;

		I2CBus(const I2CBus&) = delete;

	public:
		I2CBus(I2CBus&& bus)
			: isOpen(
				bus.isOpen
			),
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBI2C_DEV)
			fd(
				bus.fd
			),
	#endif
#endif
			path(
				Move(path)
			)
		{
			bus.isOpen = False;
		}

		explicit I2CBus(String&& path)
			: path(
				Move(path)
			)
		{
		}
		explicit I2CBus(const String& path)
			: I2CBus(
				String(path)
			)
		{
		}

		virtual ~I2CBus()
		{
			if (IsOpen())
			{

				Close();
			}
		}

		Bool IsOpen() const
		{
			return isOpen;
		}

		auto& GetPath() const
		{
			return path;
		}

		// @throw AL::Exception
		Void Open()
		{
			AL_ASSERT(
				!IsOpen(),
				"I2CBus already open"
			);

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBI2C_DEV)
			if ((fd = ::open(GetPath().GetCString(), O_RDWR)) == -1)
			{

				throw OS::SystemException(
					"open"
				);
			}
	#else
			throw DependencyMissingException(
				"libi2c-dev"
			);
	#endif
#else
			throw PlatformNotSupportedException();
#endif

			isOpen = True;
		}

		Void Close()
		{
			if (IsOpen())
			{
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBI2C_DEV)
				::close(
					fd
				);
	#endif
#endif

				isOpen = False;
			}
		}

		// @throw AL::Exception
		template<typename T, size_t S = sizeof(T)>
		Void Read(I2CAddress address, T& value)
		{
			AL_ASSERT(
				IsOpen(),
				"I2CBus not open"
			);

			Read(
				address,
				&value,
				S
			);
		}
		// @throw AL::Exception
		Void Read(I2CAddress address, Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"I2CBus not open"
			);

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBI2C_DEV)
			::i2c_msg i2c_buffer;
			i2c_buffer.flags = I2C_M_RD;
			i2c_buffer.addr = static_cast<__u16>(address);
			i2c_buffer.buf = reinterpret_cast<__u8*>(lpBuffer);
			i2c_buffer.len = static_cast<__u16>(size);

			::i2c_rdwr_ioctl_data i2c_data;
			i2c_data.msgs = &i2c_buffer;
			i2c_data.nmsgs = 1;

			if (::ioctl(fd, I2C_RDWR, &i2c_data) < 0)
			{

				throw OS::SystemException(
					"ioctl"
				);
			}
	#else
			throw NotImplementedException();
	#endif
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		template<typename T, size_t S = sizeof(T)>
		Void Write(I2CAddress address, const T& value)
		{
			AL_ASSERT(
				IsOpen(),
				"I2CBus not open"
			);

			Write(
				address,
				&value,
				S
			);
		}
		// @throw AL::Exception
		Void Write(I2CAddress address, const Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"I2CBus not open"
			);

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBI2C_DEV)
			::i2c_msg i2c_buffer;
			i2c_buffer.flags = 0;
			i2c_buffer.addr = static_cast<__u16>(address);
			i2c_buffer.buf = reinterpret_cast<__u8*>(const_cast<Void*>(lpBuffer));
			i2c_buffer.len = static_cast<__u16>(size);

			::i2c_rdwr_ioctl_data i2c_data;
			i2c_data.msgs = &i2c_buffer;
			i2c_data.nmsgs = 1;

			if (::ioctl(fd, I2C_RDWR, &i2c_data) < 0)
			{

				throw OS::SystemException(
					"ioctl"
				);
			}
	#else
			throw NotImplementedException();
	#endif
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		template<typename T_RX, typename T_TX, size_t S_RX = sizeof(T_RX), size_t S_TX = sizeof(T_TX)>
		Void WriteRead(I2CAddress address, const T_TX& tx, T_RX& rx)
		{
			AL_ASSERT(
				IsOpen(),
				"I2CBus not open"
			);

			WriteRead(
				address,
				&tx,
				S_TX,
				&rx,
				S_RX
			);
		}
		// @throw AL::Exception
		Void WriteRead(I2CAddress address, const Void* lpTX, size_t txSize, Void* lpRX, size_t rxSize)
		{
			AL_ASSERT(
				IsOpen(),
				"I2CBus not open"
			);

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBI2C_DEV)
			::i2c_msg i2c_buffers[2];
				
			// TX
			{
				i2c_buffers[0].flags = 0;
				i2c_buffers[0].addr = static_cast<__u16>(address);
				i2c_buffers[0].buf = reinterpret_cast<__u8*>(const_cast<Void*>(lpTX));
				i2c_buffers[0].len = static_cast<__u16>(txSize);
			}

			// RX
			{
				i2c_buffers[1].flags = I2C_M_RD | I2C_M_NOSTART;
				i2c_buffers[1].addr = static_cast<__u16>(address);
				i2c_buffers[1].buf = reinterpret_cast<__u8*>(lpRX);
				i2c_buffers[1].len = static_cast<__u16>(rxSize);
			}

			::i2c_rdwr_ioctl_data i2c_data;
			i2c_data.msgs = i2c_buffers;
			i2c_data.nmsgs = 2;

			if (::ioctl(fd, I2C_RDWR, &i2c_data) < 0)
			{

				throw OS::SystemException(
					"ioctl"
				);
			}
	#else
			throw NotImplementedException();
	#endif
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		template<size_t S>
		Void Execute(const I2CTransaction(&transactions)[S])
		{
			AL_ASSERT(
				IsOpen(),
				"I2CBus not open"
			);

			Execute(
				&transactions[0],
				S
			);
		}
		// @throw AL::Exception
		Void Execute(const I2CTransaction* lpTransactions, size_t count)
		{
			AL_ASSERT(
				IsOpen(),
				"I2CBus not open"
			);

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBI2C_DEV)
			Collections::Array<::i2c_msg> i2c_buffers(
				count
			);

			auto lpI2C_buffer = &i2c_buffers[0];
			auto lpTransaction = lpTransactions;

			for (size_t i = 0; i < count; ++i, ++lpI2C_buffer, ++lpTransaction)
			{
				if (lpTransaction->Flags.IsSet(I2CTransactionFlags::Read))
				{
						
					lpI2C_buffer->flags |= I2C_M_RD;
				}

				if (lpTransaction->Flags.IsSet(I2CTransactionFlags::Stop))
				{

					lpI2C_buffer->flags |= I2C_M_STOP;
				}

				if (lpTransaction->Flags.IsSet(I2CTransactionFlags::TenBit))
				{

					lpI2C_buffer->flags |= I2C_M_TEN;
				}

				if (lpTransaction->Flags.IsSet(I2CTransactionFlags::NoStart))
				{

					lpI2C_buffer->flags |= I2C_M_NOSTART;
				}

				if (lpTransaction->Flags.IsSet(I2CTransactionFlags::NoReadACK))
				{

					lpI2C_buffer->flags |= I2C_M_NO_RD_ACK;
				}

				if (lpTransaction->Flags.IsSet(I2CTransactionFlags::IgnoreNAK))
				{

					lpI2C_buffer->flags |= I2C_M_IGNORE_NAK;
				}

				lpI2C_buffer->addr = static_cast<__u16>(
					lpTransaction->Address
				);

				lpI2C_buffer->buf = reinterpret_cast<__u8*>(
					lpTransaction->lpBuffer
				);

				lpI2C_buffer->len = static_cast<__u16>(
					lpTransaction->BufferSize
				);
			}

			::i2c_rdwr_ioctl_data i2c_data;
			i2c_data.msgs = &i2c_buffers[0];
			i2c_data.nmsgs = static_cast<__u32>(count);

			if (::ioctl(fd, I2C_RDWR, &i2c_data) < 0)
			{

				throw OS::SystemException(
					"ioctl"
				);
			}
	#else
			throw NotImplementedException();
	#endif
#else
			throw NotImplementedException();
#endif
		}

		I2CBus& operator = (I2CBus&& bus)
		{
			Close();

			isOpen = bus.isOpen;
			bus.isOpen = False;

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBI2C_DEV)
			fd = bus.fd;
	#endif
#endif

			path = Move(
				bus.path
			);

			return *this;
		}

		Bool operator == (const I2CBus& bus) const
		{
			if (GetPath() != bus.GetPath())
			{

				return False;
			}

			return True;
		}
		Bool operator != (const I2CBus& bus) const
		{
			if (operator==(bus))
			{

				return False;
			}
			
			return True;
		}
	};

	class I2CDevice
	{
		I2CBus* lpBus;

		I2CAddress address;

		I2CDevice(const I2CDevice&) = delete;

	public:
		I2CDevice(I2CDevice&& device)
			: lpBus(
				device.lpBus
			),
			address(
				device.address
			)
		{
			device.lpBus = nullptr;
		}

		I2CDevice(I2CBus& bus, I2CAddress address)
			: lpBus(
				&bus
			),
			address(
				address
			)
		{
		}

		virtual ~I2CDevice()
		{
		}

		auto& GetBus()
		{
			return *lpBus;
		}
		auto& GetBus() const
		{
			return *lpBus;
		}

		auto GetAddress() const
		{
			return address;
		}
		
		// @throw AL::Exception
		template<typename T, size_t S = sizeof(T)>
		Void Read(T& value)
		{
			GetBus().Read(
				GetAddress(),
				value
			);
		}
		// @throw AL::Exception
		Void Read(Void* lpBuffer, size_t size)
		{
			GetBus().Read(
				GetAddress(),
				lpBuffer,
				size
			);
		}

		// @throw AL::Exception
		template<typename T, size_t S = sizeof(T)>
		Void Write(const T& value)
		{
			GetBus().Write(
				GetAddress(),
				value
			);
		}
		// @throw AL::Exception
		Void Write(const Void* lpBuffer, size_t size)
		{
			GetBus().Write(
				GetAddress(),
				lpBuffer,
				size
			);
		}

		// @throw AL::Exception
		template<typename T_RX, typename T_TX, size_t S_RX = sizeof(T_RX), size_t S_TX = sizeof(T_TX)>
		Void WriteRead(const T_TX& tx, T_RX& rx)
		{
			GetBus().WriteRead<T_RX, T_TX, S_RX, S_TX>(
				GetAddress(),
				tx,
				rx
			);
		}
		// @throw AL::Exception
		Void WriteRead(const Void* lpTX, size_t txSize, Void* lpRX, size_t rxSize)
		{
			GetBus().WriteRead(
				GetAddress(),
				lpTX,
				txSize,
				lpRX,
				rxSize
			);
		}

		I2CDevice& operator = (I2CDevice&& device)
		{
			lpBus = device.lpBus;
			device.lpBus = nullptr;

			address = device.address;

			return *this;
		}

		Bool operator == (const I2CDevice& device) const
		{
			if (GetBus() != device.GetBus())
			{

				return False;
			}

			if (GetAddress() != device.GetAddress())
			{

				return False;
			}

			return True;
		}
		Bool operator != (const I2CDevice& device) const
		{
			if (operator==(device))
			{

				return False;
			}

			return True;
		}
	};
}
