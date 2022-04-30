#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/Array.hpp"

#include "AL/OS/SystemException.hpp"

#if defined(AL_PLATFORM_PICO)
	#include "GPIO.hpp"

	#include <hardware/i2c.h>
#elif defined(AL_PLATFORM_LINUX)
	#include "AL/FileSystem/Path.hpp"

	#include <fcntl.h>
	#include <unistd.h>
	#include <termios.h>

	#include <sys/ioctl.h>

	#if AL_HAS_INCLUDE(<linux/i2c-dev.h>)
		#define AL_DEPENDENCY_LIBI2C_DEV

		extern "C"
		{
			#include <linux/i2c.h>
			#include <linux/i2c-dev.h>
		}
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
	#include "AL/FileSystem/Path.hpp"

	#warning Platform not supported
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

#if defined(AL_PLATFORM_PICO)
	typedef ::uint I2CBaudRate;
#endif

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
		Bool          isOpen = False;

#if defined(AL_PLATFORM_PICO)
		::i2c_inst_t* i2c;
		GPIOPin       scl;
		GPIOPin       sda;
		I2CBaudRate   baud;
#elif defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBI2C_DEV)
		int           fd;
	#endif
#endif

#if !defined(AL_PLATFORM_PICO)
		FileSystem::Path path;
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
				bus.i2c
			),
			scl(
				bus.scl
			),
			sda(
				bus.sda
			),
			baud(
				bus.baud
			)
#elif defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBI2C_DEV)
			,
			fd(
				bus.fd
			)
	#endif
#endif
#if !defined(AL_PLATFORM_PICO)
			,
			path(
				Move(path)
			)
#endif
		{
			bus.isOpen = False;

#if defined(AL_PLATFORM_PICO)
			bus.i2c = nullptr;
#endif
		}

#if defined(AL_PLATFORM_PICO)
		I2CBus(::i2c_inst_t* i2c, GPIOPin scl, GPIOPin sda, I2CBaudRate baud)
			: i2c(
				i2c
			),
			scl(
				scl
			),
			sda(
				sda
			),
			baud(
				baud
			)
		{
		}
#else
		explicit I2CBus(FileSystem::Path&& path)
			: path(
				Move(path)
			)
		{
		}
		explicit I2CBus(const FileSystem::Path& path)
			: I2CBus(
				FileSystem::Path(path)
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
		auto GetSCL() const
		{
			return scl;
		}

		auto GetSDA() const
		{
			return sda;
		}

		auto GetBaud() const
		{
			return baud;
		}

		auto GetHandle() const
		{
			return i2c;
		}
#elif defined(AL_PLATFORM_LINUX)
		auto& GetPath() const
		{
			return path;
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
			::i2c_init(
				GetHandle(),
				GetBaud()
			);

			::gpio_set_function(
				GetSDA(),
				::GPIO_FUNC_I2C
			);

			::gpio_set_function(
				GetSCL(),
				::GPIO_FUNC_I2C
			);

			::gpio_pull_up(
				GetSDA()
			);

			::gpio_pull_up(
				GetSCL()
			);
#elif defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBI2C_DEV)
			if ((fd = ::open(GetPath().GetString().GetCString(), O_RDWR)) == -1)
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
#if defined(AL_PLATFORM_PICO)
				::i2c_deinit(
					GetHandle()
				);
#elif defined(AL_PLATFORM_LINUX)
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
			if (::i2c_read_blocking(GetHandle(), address, reinterpret_cast<::uint8_t*>(lpBuffer), size, false) == PICO_ERROR_GENERIC)
			{

				throw OS::SystemException(
					"i2c_read_blocking"
				);
			}
#elif defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBI2C_DEV)
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
			if (::i2c_write_blocking(GetHandle(), address, reinterpret_cast<const ::uint8_t*>(lpBuffer), size, false) == PICO_ERROR_GENERIC)
			{

				throw OS::SystemException(
					"i2c_write_blocking"
				);
			}
#elif defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBI2C_DEV)
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
			if (::i2c_write_blocking(GetHandle(), address, reinterpret_cast<const ::uint8_t*>(lpTX), txSize, true) == PICO_ERROR_GENERIC)
			{

				throw OS::SystemException(
					"i2c_write_blocking"
				);
			}

			if (::i2c_read_blocking(GetHandle(), address, reinterpret_cast<::uint8_t*>(lpRX), rxSize, false) == PICO_ERROR_GENERIC)
			{

				throw OS::SystemException(
					"i2c_write_blocking"
				);
			}
#elif defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBI2C_DEV)
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
					if (::i2c_read_blocking(GetHandle(), lpTransactions->Address, reinterpret_cast<::uint8_t*>(lpTransactions->lpBuffer), lpTransactions->BufferSize, false) == PICO_ERROR_GENERIC)
					{

						throw OS::SystemException(
							"i2c_read_blocking"
						);
					}
				}
				else
				{
					if (::i2c_write_blocking(GetHandle(), lpTransactions->Address, reinterpret_cast<const ::uint8_t*>(lpTransactions->lpBuffer), lpTransactions->BufferSize, false) == PICO_ERROR_GENERIC)
					{

						throw OS::SystemException(
							"i2c_write_blocking"
						);
					}
				}
			}
#elif defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBI2C_DEV)
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
			i2c = bus.i2c;
			bus.i2c = nullptr;

			scl = bus.scl;
			sda = bus.sda;
#elif defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBI2C_DEV)
			fd = bus.fd;
	#endif
#endif

#if !defined(AL_PLATFORM_PICO)
			path = Move(
				bus.path
			);
#endif

			return *this;
		}

		Bool operator == (const I2CBus& bus) const
		{
#if defined(AL_PLATFORM_PICO)
			if (GetSCL() != bus.GetSCL())
			{

				return False;
			}

			if (GetSDA() != bus.GetSDA())
			{

				return False;
			}

			if (GetHandle() != bus.GetHandle())
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
