#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/Array.hpp"

#include "AL/OS/SystemException.hpp"

#if defined(AL_PLATFORM_PICO)
	#include "GPIO.hpp"

	#include "Pico/I2C.hpp"
#elif defined(AL_PLATFORM_LINUX)
	#include "AL/FileSystem/Path.hpp"

	#include <fcntl.h>
	#include <unistd.h>
	#include <termios.h>

	#include <sys/ioctl.h>

	#if !AL_HAS_INCLUDE(<linux/i2c-dev.h>)
		#error Missing linux/i2c-dev.h
	#endif

	extern "C"
	{
		#include <linux/i2c.h>
		#include <linux/i2c-dev.h>
	}
#else
	#error Platform not supported
#endif

namespace AL::Hardware
{
	typedef uint16 I2CAddress;

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
		Bool             isOpen = False;

#if defined(AL_PLATFORM_PICO)
		Pico::I2C        i2c;
#elif defined(AL_PLATFORM_LINUX)
		int              fd;
		FileSystem::Path path;
		uint32           baud;
#endif

		I2CBus(const I2CBus&) = delete;

	public:
		I2CBus(I2CBus&& bus)
			: isOpen(
				bus.isOpen
			)
#if defined(AL_PLATFORM_PICO)
			,
			i2c(
				Move(bus.i2c)
			)
#elif defined(AL_PLATFORM_LINUX)
			,
			fd(
				bus.fd
			),
			path(
				Move(path)
			),
			baud(
				bus.baud
			)
#endif
		{
			bus.isOpen = False;
		}

#if defined(AL_PLATFORM_PICO)
		I2CBus(::i2c_inst* i2c, GPIOPin scl, GPIOPin sda, uint32 baud)
			: i2c(
				i2c,
				scl,
				sda,
				baud
			)
		{
		}
#elif defined(AL_PLATFORM_LINUX)
		I2CBus(FileSystem::Path&& path, uint32 baud)
			: path(
				Move(path)
			),
			baud(
				baud
			)
		{
		}
		I2CBus(const FileSystem::Path& path, uint32 baud)
			: I2CBus(
				FileSystem::Path(path),
				baud
			)
		{
		}
#endif

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

#if defined(AL_PLATFORM_PICO)
		auto GetHandle() const
		{
			return i2c.GetI2C();
		}

		auto GetSCL() const
		{
			return static_cast<GPIOPin>(i2c.GetSCL());
		}

		auto GetSDA() const
		{
			return static_cast<GPIOPin>(i2c.GetSDA());
		}

		auto GetBaud() const
		{
			return i2c.GetBaud();
		}
#elif defined(AL_PLATFORM_LINUX)
		auto& GetPath() const
		{
			return path;
		}

		auto GetBaud() const
		{
			return baud;
		}
#endif

		// @throw AL::Exception
		Void Open()
		{
			AL_ASSERT(
				!IsOpen(),
				"I2CBus already open"
			);

#if defined(AL_PLATFORM_PICO)
			i2c.Open();
#elif defined(AL_PLATFORM_LINUX)
			if ((fd = ::open(GetPath().GetString().GetCString(), O_RDWR)) == -1)
			{

				throw OS::SystemException(
					"open"
				);
			}

			// TODO: set baud
#else
			throw PlatformNotSupportedException();
#endif

			isOpen = True;
		}

		Void Close()
		{
			if (IsOpen())
			{
#if defined(AL_PLATFORM_PICO)
				i2c.Close();
#elif defined(AL_PLATFORM_LINUX)
				::close(
					fd
				);
#endif

				isOpen = False;
			}
		}

		// @throw AL::Exception
		template<typename T, size_t S = sizeof(T)>
		Void Read(I2CAddress address, T& value) const
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
		Void Read(I2CAddress address, Void* lpBuffer, size_t size) const
		{
			AL_ASSERT(
				IsOpen(),
				"I2CBus not open"
			);

#if defined(AL_PLATFORM_PICO)
			i2c.Read(
				lpBuffer,
				size,
				address
			);
#elif defined(AL_PLATFORM_LINUX)
			::i2c_msg i2c_message =
			{
				.addr  = static_cast<__u16>(address),
				.flags = I2C_M_RD,
				.len   = static_cast<__u16>(size),
				.buf   = reinterpret_cast<__u8*>(lpBuffer)
			};

			::i2c_rdwr_ioctl_data i2c_data =
			{
				.msgs  = &i2c_message,
				.nmsgs = 1
			};

			if (::ioctl(fd, I2C_RDWR, &i2c_data) < 0)
			{

				throw OS::SystemException(
					"ioctl"
				);
			}
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

#if defined(AL_PLATFORM_PICO)
			i2c.Write(
				lpBuffer,
				size,
				address
			);
#elif defined(AL_PLATFORM_LINUX)
			::i2c_msg i2c_message =
			{
				.addr  = static_cast<__u16>(address),
				.flags = 0,
				.len   = static_cast<__u16>(size),
				.buf   = reinterpret_cast<__u8*>(const_cast<Void*>(lpBuffer))
			};

			::i2c_rdwr_ioctl_data i2c_data =
			{
				.msgs  = &i2c_message,
				.nmsgs = 1
			};

			if (::ioctl(fd, I2C_RDWR, &i2c_data) < 0)
			{

				throw OS::SystemException(
					"ioctl"
				);
			}
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

#if defined(AL_PLATFORM_PICO)
			i2c.Write(
				lpTX,
				txSize,
				address,
				True
			);

			i2c.Read(
				lpRX,
				rxSize,
				address
			);
#elif defined(AL_PLATFORM_LINUX)
			::i2c_msg i2c_messages[2] =
			{
				// TX
				{
					.addr  = static_cast<__u16>(address),
					.flags = 0,
					.len   = static_cast<__u16>(txSize),
					.buf   = reinterpret_cast<__u8*>(const_cast<Void*>(lpTX))
				},
				// RX
				{
					.addr  = static_cast<__u16>(address),
					.flags = I2C_M_RD | I2C_M_NOSTART,
					.len   = static_cast<__u16>(rxSize),
					.buf   = reinterpret_cast<__u8*>(lpRX)
				}
			};

			::i2c_rdwr_ioctl_data i2c_data =
			{
				.msgs  = &i2c_messages[0],
				.nmsgs = 2
			};

			if (::ioctl(fd, I2C_RDWR, &i2c_data) < 0)
			{

				throw OS::SystemException(
					"ioctl"
				);
			}
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

#if defined(AL_PLATFORM_PICO)
			for (size_t i = 0; i < count; ++i, ++lpTransactions)
			{
				if (lpTransactions->Flags.IsSet(I2CTransactionFlags::Stop))
				{
					// TODO: add support
					throw NotImplementedException();
				}

				if (lpTransactions->Flags.IsSet(I2CTransactionFlags::TenBit))
				{
					// TODO: add support
					throw NotImplementedException();
				}

				if (lpTransactions->Flags.IsSet(I2CTransactionFlags::NoStart))
				{
					// TODO: add support
					throw NotImplementedException();
				}

				if (lpTransactions->Flags.IsSet(I2CTransactionFlags::NoReadACK))
				{
					// TODO: add support
					throw NotImplementedException();
				}

				if (lpTransactions->Flags.IsSet(I2CTransactionFlags::IgnoreNAK))
				{
					// TODO: add support
					throw NotImplementedException();
				}

				if (lpTransactions->Flags.IsSet(I2CTransactionFlags::Read))
				{
					i2c.Read(
						lpTransactions->lpBuffer,
						lpTransactions->BufferSize,
						lpTransactions->Address
					);
				}
				else
				{
					i2c.Write(
						lpTransactions->lpBuffer,
						lpTransactions->BufferSize,
						lpTransactions->Address
					);
				}
			}
#elif defined(AL_PLATFORM_LINUX)
			Collections::Array<::i2c_msg> i2c_messages(
				count
			);

			auto lpI2C_buffer = &i2c_messages[0];
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

			::i2c_rdwr_ioctl_data i2c_data =
			{
				.msgs  = &i2c_messages[0],
				.nmsgs = static_cast<__u32>(count)
			};

			if (::ioctl(fd, I2C_RDWR, &i2c_data) < 0)
			{

				throw OS::SystemException(
					"ioctl"
				);
			}
#else
			throw NotImplementedException();
#endif
		}

		I2CBus& operator = (I2CBus&& bus)
		{
			Close();

			isOpen = bus.isOpen;
			bus.isOpen = False;

#if defined(AL_PLATFORM_PICO)
			i2c = Move(
				bus.i2c
			);
#elif defined(AL_PLATFORM_LINUX)
			fd = bus.fd;

			path = Move(
				bus.path
			);

			baud = bus.baud;
#endif

			return *this;
		}

		Bool operator == (const I2CBus& bus) const
		{
#if defined(AL_PLATFORM_PICO)
			if (i2c != bus.i2c)
			{

				return False;
			}
#elif defined(AL_PLATFORM_LINUX)
			if (GetPath() != bus.GetPath())
			{

				return False;
			}
#endif

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
		Void Read(T& value) const
		{
			GetBus().Read(
				GetAddress(),
				value
			);
		}
		// @throw AL::Exception
		Void Read(Void* lpBuffer, size_t size) const
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
