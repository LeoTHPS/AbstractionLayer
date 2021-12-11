#pragma once
#include "AL/Common.hpp"

#include "AL/FileSystem/Path.hpp"

#include "AL/OS/SystemException.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <fcntl.h>
	#include <unistd.h>
	#include <termios.h> // speed_t

	#include <sys/ioctl.h>
#elif defined(AL_PLATFORM_WINDOWS)

#endif

namespace AL::Hardware
{
	enum class UARTDeviceFlags : uint8
	{
		None         = 0x0,

		Parity       = 0x1,
		Parity_Odd   = 0x2 | Parity,
		Parity_Even  = 0x4 | Parity,

		Use2StopBits = 0x8
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(UARTDeviceFlags);

	enum class UARTDeviceSpeeds
#if defined(AL_PLATFORM_LINUX)
		: ::speed_t
#elif defined(AL_PLATFORM_WINDOWS)
		: ::DWORD
#endif
	{
#if defined(AL_PLATFORM_LINUX)
		Baud_50      = B50,
		Baud_75      = B75,
		Baud_110     = B110,
		Baud_134     = B134,
		Baud_150     = B150,
		Baud_200     = B200,
		Baud_300     = B300,
		Baud_600     = B600,
		Baud_1200    = B1200,
		Baud_1800    = B1800,
		Baud_2400    = B2400,
		Baud_4800    = B4800,
		Baud_9600    = B9600,
		Baud_19200   = B19200,
		Baud_38400   = B38400,
		Baud_57600   = B57600,
		Baud_115200  = B115200,
		Baud_230400  = B230400,
		Baud_460800  = B460800,
		Baud_500000  = B500000,
		Baud_576000  = B576000,
		Baud_921600  = B921600,
		Baud_1000000 = B1000000,
		Baud_1152000 = B1152000,
		Baud_1500000 = B1500000,
		Baud_2000000 = B2000000,
		Baud_2500000 = B2500000,
		Baud_3000000 = B3000000,
		Baud_3500000 = B3500000,
		Baud_4000000 = B4000000,
#elif defined(AL_PLATFORM_WINDOWS)
		Baud_110     = CBR_110,
		Baud_300     = CBR_300,
		Baud_600     = CBR_600,
		Baud_1200    = CBR_1200,
		Baud_2400    = CBR_2400,
		Baud_4800    = CBR_4800,
		Baud_9600    = CBR_9600,
		Baud_14400   = CBR_14400,
		Baud_19200   = CBR_19200,
		Baud_38400   = CBR_38400,
		Baud_56000   = CBR_56000,
		Baud_57600   = CBR_57600,
		Baud_115200  = CBR_115200,
		Baud_128000  = CBR_128000,
		Baud_256000  = CBR_256000,
#endif

		Default      = Baud_9600
	};

	class UARTDevice
	{
		Bool                     isOpen = False;

#if defined(AL_PLATFORM_LINUX)
		int                      fd;
#elif defined(AL_PLATFORM_WINDOWS)
		::HANDLE                 hFile;
#endif

		FileSystem::Path         path;
		BitMask<UARTDeviceFlags> flags;
		UARTDeviceSpeeds         speed;

		UARTDevice(const UARTDevice&) = delete;

	public:
		UARTDevice(UARTDevice&& device)
			: isOpen(
				device.isOpen
			),
#if defined(AL_PLATFORM_LINUX)
			fd(
				device.fd
			),
#elif defined(AL_PLATFORM_WINDOWS)
			hFile(
				device.hFile
			),
#endif
			path(
				Move(device.path)
			),
			flags(
				device.flags
			),
			speed(
				device.speed
			)
		{
			device.isOpen = False;
		}

		UARTDevice(FileSystem::Path&& path, UARTDeviceSpeeds speed, UARTDeviceFlags flags)
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
		UARTDevice(const FileSystem::Path& path, UARTDeviceSpeeds speed, UARTDeviceFlags flags)
			: UARTDevice(
				FileSystem::Path(path),
				speed,
				flags
			)
		{
		}

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

		auto& GetPath() const
		{
			return path;
		}

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

#if defined(AL_PLATFORM_LINUX)
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
#if defined(AL_PLATFORM_LINUX)
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
		Void Read(T& value)
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
		Void Read(Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"UARTDevice not open"
			);

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
		}

		UARTDevice& operator = (UARTDevice&& device)
		{
			Close();

			isOpen = device.isOpen;
			device.isOpen = False;

#if defined(AL_PLATFORM_LINUX)
			fd = device.fd;
#elif defined(AL_PLATFORM_WINDOWS)
			hFile = device.hFile;
#endif

			path = Move(
				device.path
			);

			flags = device.flags;
			speed = device.speed;

			return *this;
		}

		Bool operator == (const UARTDevice& device) const
		{
			if (GetPath() != device.GetPath())
			{
					
				return False;
			}

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
