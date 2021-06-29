#pragma once
#include "AL/Common.hpp"

#include "Pin.hpp"

#include "UARTDeviceFlags.hpp"
#include "UARTDeviceSpeeds.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <fcntl.h>
	#include <unistd.h>
	#include <termios.h>

	#include <sys/ioctl.h>
#endif

namespace AL::GPIO
{
	class UARTDevice
	{
#if defined(AL_PLATFORM_LINUX)
		typedef int HFILE;
		
		static constexpr HFILE FILE_HANDLE_NULL = -1;
#elif defined(AL_PLATFORM_WINDOWS)
		typedef HANDLE HFILE;

		static constexpr HFILE FILE_HANDLE_NULL = INVALID_HANDLE_VALUE;
#else
		typedef void* HFILE;

		static constexpr HFILE FILE_HANDLE_NULL = nullptr;
#endif

		HFILE hFile;
		String name;
		UARTDeviceSpeeds speed;
		BitMask<UARTDeviceFlags> flags;

		UARTDevice(const UARTDevice&) = delete;

		UARTDevice(HFILE hFile, String&& name, UARTDeviceSpeeds speed, UARTDeviceFlags flags)
			: hFile(
				hFile
			),
			name(
				Move(name)
			),
			speed(
				speed
			),
			flags(
				flags
			)
		{
		}

	public:
		// @throw AL::Exceptions::Exception
		static void Open(UARTDevice& device, String&& name, UARTDeviceSpeeds speed, UARTDeviceFlags flags = UARTDeviceFlags::None)
		{
			BitMask<UARTDeviceFlags> _flags(
				flags
			);

#if defined(AL_PLATFORM_LINUX)
			int fd;

			if ((fd = open(name.GetCString(), O_RDWR | O_NOCTTY | O_NDELAY)) == -1)
			{

				throw Exceptions::SystemException(
					"open"
				);
			}

			fcntl(fd, F_SETFL, O_RDWR);

			termios options;
			tcgetattr(fd, &options);
			cfmakeraw(&options);
			cfsetspeed(&options, static_cast<typename Get_Enum_Base<UARTDeviceSpeeds>::Type>(speed));

			options.c_cc[VMIN] = 1;
			options.c_cc[VTIME] = 0;

			options.c_cflag |= CS8;
			options.c_cflag |= CLOCAL | CREAD;
			options.c_cflag &= ~(PARENB | CSTOPB | CSIZE);
			options.c_cflag &= ~(ICANON | ECHO | ECHOE | ISIG | OPOST);

			if (_flags.IsSet(UARTDeviceFlags::Parity))
			{
				options.c_cflag |= PARENB;

				if (_flags.IsSet(UARTDeviceFlags::Parity_Odd))
				{

					options.c_cflag |= PARODD;
				}
			}
			
			if (_flags.IsSet(UARTDeviceFlags::Use2StopBits))
			{

				options.c_cflag |= CSTOPB;
			}

			tcsetattr(
				fd,
				TCSANOW,
				&options
			);

			int status;

			ioctl(
				fd,
				TIOCMGET,
				&status
			);
			
			status |= TIOCM_DTR | TIOCM_RTS;
			
			ioctl(
				fd,
				TIOCMSET,
				&status
			);

			device = UARTDevice(
				fd,
				Move(name),
				speed,
				flags
			);
#elif defined(AL_PLATFORM_WINDOWS)
			HANDLE hFile;

			if ((hFile = CreateFileA(name.GetCString(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0)) == INVALID_HANDLE_VALUE)
			{

				throw Exceptions::SystemException(
					"CreateFileA"
				);
			}

			DCB dcb = { 0 };
			dcb.DCBlength = sizeof(DCB);

			if (!GetCommState(hFile, &dcb))
			{
				CloseHandle(hFile);

				throw Exceptions::SystemException(
					"GetCommState"
				);
			}

			dcb.Parity = NOPARITY;
			dcb.fBinary = TRUE;
			dcb.BaudRate = static_cast<typename Get_Enum_Base<UARTDeviceSpeeds>::Type>(speed);
			dcb.ByteSize = 8;
			dcb.StopBits = ONESTOPBIT;
			
			if (_flags.IsSet(UARTDeviceFlags::Parity))
			{
				dcb.Parity = EVENPARITY;

				if (_flags.IsSet(UARTDeviceFlags::Parity_Odd))
				{

					dcb.Parity = ODDPARITY;
				}
			}

			if (_flags.IsSet(UARTDeviceFlags::Use2StopBits))
			{

				dcb.StopBits = TWOSTOPBITS;
			}

			if (!SetCommState(hFile, &dcb))
			{
				CloseHandle(hFile);

				throw Exceptions::SystemException(
					"SetCommState"
				);
			}

			COMMTIMEOUTS timeouts = { 0 };
			timeouts.ReadIntervalTimeout = MAXWORD;
			timeouts.ReadTotalTimeoutConstant = 0;
			timeouts.ReadTotalTimeoutMultiplier = 0;
			timeouts.WriteTotalTimeoutConstant = 50;
			timeouts.WriteTotalTimeoutMultiplier = 10;

			if (!SetCommTimeouts(hFile, &timeouts))
			{
				CloseHandle(hFile);

				throw Exceptions::SystemException(
					"SetCommTimeouts"
				);
			}

			device = UARTDevice(
				hFile,
				Move(name),
				speed,
				flags
			);
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		static void Open(UARTDevice& device, const String& name, UARTDeviceSpeeds speed, UARTDeviceFlags flags = UARTDeviceFlags::None)
		{
			Open(
				device,
				String(name),
				speed,
				flags
			);
		}

		UARTDevice()
			: UARTDevice(
				FILE_HANDLE_NULL,
				"",
				UARTDeviceSpeeds::Default,
				UARTDeviceFlags::None
			)
		{
		}

		UARTDevice(UARTDevice&& device)
			: hFile(
				device.hFile
			),
			name(
				Move(device.name)
			),
			speed(
				device.speed
			),
			flags(
				Move(device.flags)
			)
		{
			device.hFile = FILE_HANDLE_NULL;
			device.speed = UARTDeviceSpeeds::Default;
		}

		virtual ~UARTDevice()
		{
			Close();
		}

		bool IsOpen() const
		{
			return hFile != FILE_HANDLE_NULL;
		}

		auto& GetName() const
		{
			return name;
		}

		auto GetFlags() const
		{
			return flags;
		}

		auto GetSpeed() const
		{
			return speed;
		}

		// @throw AL::Exceptions::Exception
		template<typename T, size_t S = sizeof(T)>
		void Read(T& value)
		{
			AL_ASSERT(IsOpen(), "UARTDevice not open");

			Read(
				&value,
				S
			);
		}
		// @throw AL::Exceptions::Exception
		void Read(void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "UARTDevice not open");

			for (size_t totalBytesRead = 0; totalBytesRead < size; )
			{
#if defined(AL_PLATFORM_LINUX)
				int bytesRead;

				if ((bytesRead = read(hFile, &reinterpret_cast<uint8*>(lpBuffer)[totalBytesRead], size - totalBytesRead)) == -1)
				{

					throw Exceptions::SystemException(
						"read"
					);
				}
#elif defined(AL_PLATFORM_WINDOWS)
				DWORD bytesRead;

				if (!ReadFile(hFile, &reinterpret_cast<uint8*>(lpBuffer)[totalBytesRead], static_cast<DWORD>(size - totalBytesRead), &bytesRead, nullptr))
				{

					throw Exceptions::SystemException(
						"ReadFile"
					);
				}
#else
				throw Exceptions::NotImplementedException();
#endif

				totalBytesRead += bytesRead;
			}
		}

		// @throw AL::Exceptions::Exception
		template<typename T, size_t S = sizeof(T)>
		void Write(const T& value)
		{
			AL_ASSERT(IsOpen(), "UARTDevice not open");

			Write(
				&value,
				S
			);
		}
		// @throw AL::Exceptions::Exception
		void Write(const void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "UARTDevice not open");

			for (size_t totalBytesWritten = 0; totalBytesWritten < size; )
			{
#if defined(AL_PLATFORM_LINUX)
				int bytesWritten;

				if ((bytesWritten = write(hFile, &reinterpret_cast<const uint8*>(lpBuffer)[totalBytesWritten], size - totalBytesWritten)) == -1)
				{

					throw Exceptions::SystemException(
						"write"
					);
				}
#elif defined(AL_PLATFORM_WINDOWS)
				DWORD bytesWritten;

				if (!WriteFile(hFile, &reinterpret_cast<const uint8*>(lpBuffer)[totalBytesWritten], static_cast<DWORD>(size - totalBytesWritten), &bytesWritten, nullptr))
				{

					throw Exceptions::SystemException(
						"WriteFile"
					);
				}
#else
				throw Exceptions::NotImplementedException();
#endif

				totalBytesWritten += bytesWritten;
			}
		}

		void Close()
		{
			if (IsOpen())
			{
#if defined(AL_PLATFORM_LINUX)
				close(hFile);
#elif defined(AL_PLATFORM_WINDOWS)
				CloseHandle(hFile);
#endif

				hFile = FILE_HANDLE_NULL;
			}
		}

		UARTDevice& operator = (UARTDevice&& device)
		{
			Close();

			hFile = device.hFile;
			device.hFile = FILE_HANDLE_NULL;

			name = Move(
				device.name
			);

			speed = device.speed;
			device.speed = UARTDeviceSpeeds::Default;

			flags = Move(
				device.flags
			);

			return *this;
		}
	};
}
