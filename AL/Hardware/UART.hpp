#pragma once
#include "AL/Common.hpp"

#include "AL/OS/SystemException.hpp"

#if defined(AL_PLATFORM_PICO)
	#include "Pico/UART.hpp"
#elif defined(AL_PLATFORM_LINUX)
	#include "AL/FileSystem/Path.hpp"

	#include <fcntl.h>
	#include <unistd.h>
	#include <termios.h> // speed_t

	#include <sys/ioctl.h>
#elif defined(AL_PLATFORM_WINDOWS)
	#include "AL/FileSystem/Path.hpp"
#else
	#error Platform not supported
#endif

namespace AL::Hardware
{
	enum class UARTDeviceFlags : uint8
	{
		None         = 0x00,

		Parity       = 0x01,
		Parity_Odd   = 0x02 | Parity,
		Parity_Even  = 0x04 | Parity,

		Use2StopBits = 0x08
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(UARTDeviceFlags);

	class UARTDevice
	{
		Bool             isOpen = False;

#if defined(AL_PLATFORM_PICO)
		Pico::UART       uart;
#elif defined(AL_PLATFORM_LINUX)
		FileSystem::Path path;
		int              fd;
#elif defined(AL_PLATFORM_WINDOWS)
		FileSystem::Path path;
		::HANDLE         hFile;
#endif

		BitMask<UARTDeviceFlags> flags;
		uint32                   speed;

		UARTDevice(const UARTDevice&) = delete;

	public:
		UARTDevice(UARTDevice&& device)
			: isOpen(
				device.isOpen
			),
#if defined(AL_PLATFORM_PICO)
			uart(
				Move(device.uart)
			),
#elif defined(AL_PLATFORM_LINUX)
			fd(
				device.fd
			),
			path(
				Move(device.path)
			),
#elif defined(AL_PLATFORM_WINDOWS)
			path(
				Move(device.path)
			),
			hFile(
				device.hFile
			),
#endif
			flags(
				device.flags
			),
			speed(
				device.speed
			)
		{
			device.isOpen = False;
		}

#if defined(AL_PLATFORM_PICO)
		UARTDevice(::uart_inst_t* uart, GPIOPin rx, GPIOPin tx, uint32 speed, UARTDeviceFlags flags)
			: uart(
				uart,
				rx,
				tx,
				speed,
				[flags]()
				{
					BitMask<Pico::UARTFlags> mask;
					mask.Set(Pico::UARTFlags::Parity_Odd, BitMask<UARTDeviceFlags>::IsSet(flags, UARTDeviceFlags::Parity_Odd));
					mask.Set(Pico::UARTFlags::Parity_Even, BitMask<UARTDeviceFlags>::IsSet(flags, UARTDeviceFlags::Parity_Even));
					mask.Set(Pico::UARTFlags::Use2StopBits, BitMask<UARTDeviceFlags>::IsSet(flags, UARTDeviceFlags::Use2StopBits));

					return mask.Value;
				}()
			),
			flags(
				flags
			),
			speed(
				speed
			)
		{
		}
#elif defined(AL_PLATFORM_LINUX)
		UARTDevice(FileSystem::Path&& path, uint32 speed, UARTDeviceFlags flags)
			: path(
				Move(path)
			),
			flags(
				flags
			),
			speed(
				speed
			)
		{
		}
		UARTDevice(const FileSystem::Path& path, uint32 speed, UARTDeviceFlags flags)
			: UARTDevice(
				FileSystem::Path(path),
				speed,
				flags
			)
		{
		}
#elif defined(AL_PLATFORM_WINDOWS)
		UARTDevice(FileSystem::Path&& path, uint32 speed, UARTDeviceFlags flags)
			: path(
				Move(path)
			),
			flags(
				flags
			),
			speed(
				speed
			)
		{
		}
		UARTDevice(const FileSystem::Path& path, uint32 speed, UARTDeviceFlags flags)
			: UARTDevice(
				FileSystem::Path(path),
				speed,
				flags
			)
		{
		}
#endif

		virtual ~UARTDevice()
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
		auto GetRX() const
		{
			return uart.GetRX();
		}

		auto GetTX() const
		{
			return uart.GetTX();
		}

		auto GetHandle() const
		{
			return uart.GetUART();
		}
#elif defined(AL_PLATFORM_LINUX)
		auto& GetPath() const
		{
			return path;
		}
#elif defined(AL_PLATFORM_WINDOWS)
		auto& GetPath() const
		{
			return path;
		}
#endif

		auto GetSpeed() const
		{
			return speed;
		}

		auto& GetFlags() const
		{
			return flags;
		}

		// @throw AL::Exception
		Void Open()
		{
			AL_ASSERT(
				!IsOpen(),
				"UARTDevice already open"
			);

#if defined(AL_PLATFORM_PICO)
			uart.Open();
#elif defined(AL_PLATFORM_LINUX)
			if ((fd = ::open(GetPath().GetString().GetCString(), O_RDWR | O_NOCTTY | O_NDELAY)) == -1)
			{

				throw OS::SystemException(
					"open"
				);
			}

			::fcntl(
				fd,
				F_SETFL,
				O_RDWR
			);

			::termios options;

			::tcgetattr(
				fd,
				&options
			);

			::cfmakeraw(
				&options
			);

			::cfsetspeed(
				&options,
				static_cast<typename Get_Enum_Or_Integer_Base<UARTDeviceSpeeds>::Type>(GetSpeed())
			);

			options.c_cc[VMIN] = 1;
			options.c_cc[VTIME] = 0;

			options.c_cflag |= CS8;
			options.c_cflag |= CLOCAL | CREAD;
			options.c_cflag &= ~(PARENB | CSTOPB | CSIZE);
			options.c_cflag &= ~(ICANON | ECHO | ECHOE | ISIG | OPOST);

			if (GetFlags().IsSet(UARTDeviceFlags::Parity))
			{
				options.c_cflag |= PARENB;

				if (GetFlags().IsSet(UARTDeviceFlags::Parity_Odd))
				{

					options.c_cflag |= PARODD;
				}
			}

			if (GetFlags().IsSet(UARTDeviceFlags::Use2StopBits))
			{

				options.c_cflag |= CSTOPB;
			}

			::tcsetattr(
				fd,
				TCSANOW,
				&options
			);

			int status;

			::ioctl(
				fd,
				TIOCMGET,
				&status
			);

			status |= TIOCM_DTR | TIOCM_RTS;

			::ioctl(
				fd,
				TIOCMSET,
				&status
			);
#elif defined(AL_PLATFORM_WINDOWS)
			if ((hFile = ::CreateFileA(GetPath().GetString().GetCString(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0)) == INVALID_HANDLE_VALUE)
			{

				throw OS::SystemException(
					"CreateFileA"
				);
			}

			::DCB dcb = { };
			dcb.DCBlength = sizeof(::DCB);

			if (!::GetCommState(hFile, &dcb))
			{
				::CloseHandle(
					hFile
				);

				throw OS::SystemException(
					"GetCommState"
				);
			}

			dcb.Parity   = NOPARITY;
			dcb.fBinary  = TRUE;
			dcb.BaudRate = static_cast<::DWORD>(GetSpeed());
			dcb.ByteSize = 8;
			dcb.StopBits = ONESTOPBIT;

			if (GetFlags().IsSet(UARTDeviceFlags::Parity))
			{
				dcb.Parity = EVENPARITY;

				if (GetFlags().IsSet(UARTDeviceFlags::Parity_Odd))
				{

					dcb.Parity = ODDPARITY;
				}
			}

			if (GetFlags().IsSet(UARTDeviceFlags::Use2StopBits))
			{

				dcb.StopBits = TWOSTOPBITS;
			}

			if (!::SetCommState(hFile, &dcb))
			{
				::CloseHandle(
					hFile
				);

				throw OS::SystemException(
					"SetCommState"
				);
			}

			::COMMTIMEOUTS timeouts              = { };
			timeouts.ReadIntervalTimeout         = MAXWORD;
			timeouts.ReadTotalTimeoutConstant    = 0;
			timeouts.ReadTotalTimeoutMultiplier  = 0;
			timeouts.WriteTotalTimeoutConstant   = 50;
			timeouts.WriteTotalTimeoutMultiplier = 10;

			if (!::SetCommTimeouts(hFile, &timeouts))
			{
				::CloseHandle(
					hFile
				);

				throw OS::SystemException(
					"SetCommTimeouts"
				);
			}
#endif

			isOpen = True;
		}

		Void Close()
		{
			if (IsOpen())
			{
#if defined(AL_PLATFORM_PICO)
				uart.Close();
#elif defined(AL_PLATFORM_LINUX)
				::close(
					fd
				);
#elif defined(AL_PLATFORM_WINDOWS)
				::CloseHandle(
					hFile
				);
#endif

				isOpen = False;
			}
		}

		// @throw AL::Exception
		template<typename T, size_t S = sizeof(T)>
		Void Read(T& value) const
		{
			AL_ASSERT(
				IsOpen(),
				"UARTDevice not open"
			);

			Read(
				&value,
				S
			);
		}
		// @throw AL::Exception
		Void Read(Void* lpBuffer, size_t size) const
		{
			AL_ASSERT(
				IsOpen(),
				"UARTDevice not open"
			);

#if defined(AL_PLATFORM_PICO)
			uart.Read(
				lpBuffer,
				size
			);
#else
			for (size_t totalBytesRead = 0; totalBytesRead < size; )
			{
	#if defined(AL_PLATFORM_LINUX)
				int bytesRead;

				if ((bytesRead = ::read(fd, &reinterpret_cast<uint8*>(lpBuffer)[totalBytesRead], size - totalBytesRead)) == -1)
				{

					throw OS::SystemException(
						"read"
					);
				}

				totalBytesRead += bytesRead;
	#elif defined(AL_PLATFORM_WINDOWS)
				::DWORD bytesRead;

				if (!::ReadFile(hFile, &reinterpret_cast<uint8*>(lpBuffer)[totalBytesRead], static_cast<::DWORD>(size - totalBytesRead), &bytesRead, nullptr))
				{

					throw OS::SystemException(
						"ReadFile"
					);
				}

				totalBytesRead += bytesRead;
	#endif
			}
#endif
		}

		// @throw AL::Exception
		template<typename T, size_t S = sizeof(T)>
		Void Write(const T& value)
		{
			AL_ASSERT(
				IsOpen(),
				"UARTDevice not open"
			);

			Write(
				&value,
				S
			);
		}
		// @throw AL::Exception
		Void Write(const Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"UARTDevice not open"
			);

#if defined(AL_PLATFORM_PICO)
			uart.Write(
				lpBuffer,
				size
			);
#else
			for (size_t totalBytesWritten = 0; totalBytesWritten < size; )
			{
	#if defined(AL_PLATFORM_LINUX)
				int bytesWritten;

				if ((bytesWritten = ::write(fd, &reinterpret_cast<const uint8*>(lpBuffer)[totalBytesWritten], size - totalBytesWritten)) == -1)
				{

					throw OS::SystemException(
						"write"
					);
				}

				totalBytesWritten += bytesWritten;
	#elif defined(AL_PLATFORM_WINDOWS)
				::DWORD bytesWritten;

				if (!::WriteFile(hFile, &reinterpret_cast<const uint8*>(lpBuffer)[totalBytesWritten], static_cast<::DWORD>(size - totalBytesWritten), &bytesWritten, nullptr))
				{

					throw OS::SystemException(
						"WriteFile"
					);
				}

				totalBytesWritten += bytesWritten;
	#endif
			}
#endif
		}

		// @throw AL::Exception
		// @return number of bytes read
		template<typename T, size_t S = sizeof(T)>
		size_t TryRead(T& value)
		{
			AL_ASSERT(
				IsOpen(),
				"UARTDevice not open"
			);

			auto numberOfBytesRead = TryRead(
				&value,
				S
			);

			return numberOfBytesRead;
		}
		// @throw AL::Exception
		// @return number of bytes read
		size_t TryRead(Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"UARTDevice not open"
			);

#if defined(AL_PLATFORM_PICO)
			size_t bytesRead = 0;

			for (size_t i = 0; (i < size) && uart.IsReadable(); ++i, ++bytesRead)
			{
				uart.Read(
					&reinterpret_cast<uint8*>(lpBuffer)[i],
					1
				);
			}
#elif defined(AL_PLATFORM_LINUX)
			if (size > Integer<::size_t>::Maximum)
			{

				size = Integer<::size_t>::Maximum;
			}

			::ssize_t bytesRead;

			if ((bytesRead = ::read(fd, lpBuffer, size)) == -1)
			{

				throw OS::SystemException(
					"read"
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			if (size > Integer<::DWORD>::Maximum)
			{

				size = Integer<::DWORD>::Maximum;
			}

			::DWORD bytesRead;

			if (!::ReadFile(hFile, lpBuffer, static_cast<::DWORD>(size), &bytesRead, nullptr))
			{

				throw OS::SystemException(
					"ReadFile"
				);
			}
#endif

			return static_cast<size_t>(
				bytesRead
			);
		}

		UARTDevice& operator = (UARTDevice&& device)
		{
			Close();

			isOpen = device.isOpen;
			device.isOpen = False;

#if defined(AL_PLATFORM_PICO)
			uart = Move(
				device.uart
			);
#elif defined(AL_PLATFORM_LINUX)
			path = Move(
				device.path
			);

			fd = device.fd;
#elif defined(AL_PLATFORM_WINDOWS)
			path = Move(
				device.path
			);

			hFile = device.hFile;
#endif

			flags = device.flags;
			speed = device.speed;

			return *this;
		}

		Bool operator == (const UARTDevice& device) const
		{
#if defined(AL_PLATFORM_PICO)
			if (uart != device.uart)
			{

				return False;
			}
#elif defined(AL_PLATFORM_LINUX)
			if (GetPath() != device.GetPath())
			{

				return False;
			}
#elif defined(AL_PLATFORM_WINDOWS)
			if (GetPath() != device.GetPath())
			{

				return False;
			}
#endif

			return True;
		}
		Bool operator != (const UARTDevice& device) const
		{
			if (operator==(device))
			{

				return False;
			}

			return True;
		}
	};
}
