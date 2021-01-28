#pragma once
#include "AL/Common.hpp"

#include "Pin.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <fcntl.h>
	#include <unistd.h>
	#include <termios.h>

	#include <sys/ioctl.h>
#endif

#if defined(AL_PLATFORM_LINUX)
	#define AL_GPIO_UART_DEVICE_SPEED_50      B50
	#define AL_GPIO_UART_DEVICE_SPEED_75      B75
	#define AL_GPIO_UART_DEVICE_SPEED_110     B110
	#define AL_GPIO_UART_DEVICE_SPEED_134     B134
	#define AL_GPIO_UART_DEVICE_SPEED_150     B150
	#define AL_GPIO_UART_DEVICE_SPEED_200     B200
	#define AL_GPIO_UART_DEVICE_SPEED_300     B300
	#define AL_GPIO_UART_DEVICE_SPEED_600     B600
	#define AL_GPIO_UART_DEVICE_SPEED_1200    B1200
	#define AL_GPIO_UART_DEVICE_SPEED_1800    B1800
	#define AL_GPIO_UART_DEVICE_SPEED_2400    B2400
	#define AL_GPIO_UART_DEVICE_SPEED_4800    B4800
	#define AL_GPIO_UART_DEVICE_SPEED_9600    B9600
	#define AL_GPIO_UART_DEVICE_SPEED_14400   B14400
	#define AL_GPIO_UART_DEVICE_SPEED_19200   B19200
	#define AL_GPIO_UART_DEVICE_SPEED_38400   B38400
	#define AL_GPIO_UART_DEVICE_SPEED_56000   B56000
	#define AL_GPIO_UART_DEVICE_SPEED_57600   B57600
	#define AL_GPIO_UART_DEVICE_SPEED_115200  B115200
	#define AL_GPIO_UART_DEVICE_SPEED_128000  B128000
	#define AL_GPIO_UART_DEVICE_SPEED_230400  B230400
	#define AL_GPIO_UART_DEVICE_SPEED_256000  B256000
	#define AL_GPIO_UART_DEVICE_SPEED_460800  B460800
	#define AL_GPIO_UART_DEVICE_SPEED_500000  B500000
	#define AL_GPIO_UART_DEVICE_SPEED_576000  B576000
	#define AL_GPIO_UART_DEVICE_SPEED_921600  B921600
	#define AL_GPIO_UART_DEVICE_SPEED_1000000 B1000000
	#define AL_GPIO_UART_DEVICE_SPEED_1152000 B1152000
	#define AL_GPIO_UART_DEVICE_SPEED_1500000 B1500000
	#define AL_GPIO_UART_DEVICE_SPEED_2000000 B2000000
	#define AL_GPIO_UART_DEVICE_SPEED_2500000 B2500000
	#define AL_GPIO_UART_DEVICE_SPEED_3000000 B3000000
	#define AL_GPIO_UART_DEVICE_SPEED_3500000 B3500000
	#define AL_GPIO_UART_DEVICE_SPEED_4000000 B4000000
#elif defined(AL_PLATFORM_WINDOWS)
	#define AL_GPIO_UART_DEVICE_SPEED_110     CBR_110
	#define AL_GPIO_UART_DEVICE_SPEED_300     CBR_300
	#define AL_GPIO_UART_DEVICE_SPEED_600     CBR_600
	#define AL_GPIO_UART_DEVICE_SPEED_1200    CBR_1200
	#define AL_GPIO_UART_DEVICE_SPEED_2400    CBR_2400
	#define AL_GPIO_UART_DEVICE_SPEED_4800    CBR_4800
	#define AL_GPIO_UART_DEVICE_SPEED_9600    CBR_9600
	#define AL_GPIO_UART_DEVICE_SPEED_14400   CBR_14400
	#define AL_GPIO_UART_DEVICE_SPEED_19200   CBR_19200
	#define AL_GPIO_UART_DEVICE_SPEED_38400   CBR_38400
	#define AL_GPIO_UART_DEVICE_SPEED_56000   CBR_56000
	#define AL_GPIO_UART_DEVICE_SPEED_57600   CBR_57600
	#define AL_GPIO_UART_DEVICE_SPEED_115200  CBR_115200
	#define AL_GPIO_UART_DEVICE_SPEED_128000  CBR_128000
	#define AL_GPIO_UART_DEVICE_SPEED_256000  CBR_256000
#endif

namespace AL::GPIO
{
#if defined(AL_PLATFORM_LINUX)
	typedef speed_t UARTDeviceSpeed;
#else
	typedef uint32 UARTDeviceSpeed;
#endif

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
		UARTDeviceSpeed speed;

		UARTDevice(const UARTDevice&) = delete;

		UARTDevice(HANDLE hFile, String&& name, UARTDeviceSpeed speed)
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
		static void Open(UARTDevice& device, String&& name, UARTDeviceSpeed speed)
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
			cfsetispeed(&options, speed);
			cfsetospeed(&options, speed);

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

			dcb.BaudRate = speed;
			dcb.ByteSize = 8;
			dcb.Parity = NOPARITY;
			dcb.fBinary = TRUE;
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
		static void Open(UARTDevice& device, const String& name, UARTDeviceSpeed speed)
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
				0
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
		}

		UARTDevice(String&& name, UARTDeviceSpeed speed)
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
			size_t bytesRead;

			if ((bytesRead = read(hFile, lpBuffer, size)) == -1)
			{

				throw Exceptions::SystemException(
					"read"
				);
			}

			return bytesRead;
#elif defined(AL_PLATFORM_WINDOWS)
			DWORD bytesRead;

			if (!ReadFile(hFile, lpBuffer, static_cast<DWORD>(size), &bytesRead, nullptr))
			{

				throw Exceptions::SystemException(
					"ReadFile"
				);
			}

			return bytesRead;
#else
			throw Exceptions::NotImplementedException();
#endif
		}

		// @throw AL::Exceptions::Exception
		void Write(const void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "UARTDevice not open");

#if defined(AL_PLATFORM_LINUX)
			if (write(fd, lpBuffer, size) == -1)
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
			device.speed = 0;

			return *this;
		}
	};
}
