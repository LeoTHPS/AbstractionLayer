#pragma once
#include "AL/Common.hpp"

#include "Pin.hpp"

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

		UARTDevice(const UARTDevice&) = delete;

		UARTDevice(HFILE hFile, String&& name, UARTDeviceSpeeds speed)
			: hFile(
				hFile
			),
			name(
				Move(name)
			),
			speed(
				speed
			)
		{
		}

	public:
		// @throw AL::Exceptions::Exception
		static void Open(UARTDevice& device, String&& name, UARTDeviceSpeeds speed)
		{
#if defined(AL_PLATFORM_LINUX)
			int fd;
			int status;

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
			cfsetispeed(&options, static_cast<typename Get_Enum_Base<UARTDeviceSpeeds>::Type>(speed));
			cfsetospeed(&options, static_cast<typename Get_Enum_Base<UARTDeviceSpeeds>::Type>(speed));

			options.c_cc[VMIN] = 0;
			options.c_cc[VTIME] = 0;

			options.c_cflag |= CLOCAL | CREAD;
			options.c_cflag &= ~(PARENB | CSTOPB | CSIZE);
			options.c_cflag |= CS8;
			options.c_cflag &= ~(ICANON | ECHO | ECHOE | ISIG | OPOST);

			tcsetattr(fd, TCSANOW, &options);

			ioctl(fd, TIOCMGET, &status);
			status |= TIOCM_DTR | TIOCM_RTS;
			ioctl(fd, TIOCMSET, &status);

			device = UARTDevice(
				fd,
				Move(name),
				speed
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
				speed
			);
#else
			throw Exceptions::NotImplementedException();
#endif
		}
		// @throw AL::Exceptions::Exception
		static void Open(UARTDevice& device, const String& name, UARTDeviceSpeeds speed)
		{
			Open(
				device,
				String(name),
				speed
			);
		}

		UARTDevice()
			: UARTDevice(
				FILE_HANDLE_NULL,
				"",
				UARTDeviceSpeeds::Default
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
			)
		{
			device.hFile = FILE_HANDLE_NULL;
			device.speed = UARTDeviceSpeeds::Default;
		}

		UARTDevice(String&& name, UARTDeviceSpeeds speed)
			: hFile(
				FILE_HANDLE_NULL
			),
			name(
				Move(name)
			),
			speed(
				speed
			)
		{
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

		auto GetSpeed() const
		{
			return speed;
		}

		// @throw AL::Exceptions::Exception
		// @return number of bytes read
		size_t Read(void* lpBuffer, size_t size)
		{
#if defined(AL_PLATFORM_LINUX)
			int bytesRead;

			if ((bytesRead = read(hFile, lpBuffer, size)) == -1)
			{

				throw Exceptions::SystemException(
					"read"
				);
			}

			return static_cast<size_t>(
				bytesRead
			);
#elif defined(AL_PLATFORM_WINDOWS)
			DWORD bytesRead;

			if (!ReadFile(hFile, lpBuffer, static_cast<DWORD>(size), &bytesRead, nullptr))
			{

				throw Exceptions::SystemException(
					"ReadFile"
				);
			}

			return static_cast<size_t>(
				bytesRead
			);
#else
			throw Exceptions::NotImplementedException();
#endif
		}

		// @throw AL::Exceptions::Exception
		void Write(const void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "UARTDevice not open");

#if defined(AL_PLATFORM_LINUX)
			if (write(hFile, lpBuffer, size) == -1)
			{

				throw Exceptions::SystemException(
					"write"
				);
			}
#elif defined(AL_PLATFORM_WINDOWS)
			DWORD bytesWritten;

			if (!WriteFile(hFile, lpBuffer, static_cast<DWORD>(size), &bytesWritten, nullptr))
			{

				throw Exceptions::SystemException(
					"WriteFile"
				);
			}
#else
			throw Exceptions::NotImplementedException();
#endif
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

			return *this;
		}
	};
}
