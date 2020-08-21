#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_LINUX)
	#error Platform not supported
#endif

#include "Pin.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#include <sys/ioctl.h>

namespace AL::GPIO
{
	typedef speed_t UARTDeviceSpeed;

	class UARTDevice
	{
		int fd;
		String name;
		UARTDeviceSpeed speed;

		UARTDevice(const UARTDevice&) = delete;

		UARTDevice(int fd, String&& name, UARTDeviceSpeed speed)
			: fd(
				fd
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
			int fd;
			int status;

			if ((fd = open(name.GetCString(), O_RDWR | O_NOCTTY | O_NDELAY)) == -1)
			{

				throw Exceptions::Exception(
					Exceptions::SystemException(
						"open"
					),
					"Error opening '%s'",
					name.GetCString()
				);
			}

			fcntl(fd, F_SETFL, O_RDWR);

			termios options;
			speed_t baudRate;
			
			if (!SpeedToBaud(baudRate, speed))
			{
				close(fd);
				
				throw Exceptions::Exception(
					"Invalid UARTDeviceSpeed"
				);
			}

			tcgetattr(fd, &options);
			cfmakeraw(&options);
			cfsetispeed(&options, baudRate);
			cfsetospeed(&options, baudRate);

			options.c_cc[VMIN] = 0;
			options.c_cc[VTIME] = 50; // 5 seconds (deciseconds)

			options.c_cflag |= CLOCAL | CREAD;
			options.c_cflag &= ~(PARENB | CSTOPB | CSIZE);
			options.c_cflag |= CS8;
			options.c_cflag &= ~(ICANON | ECHO | ECHOE | ISIG | OPOST);
			
			tcsetattr(fd, TCSANOW, &options);

			ioctl(fd, TIOCMGET, &status);
			status |= TIOCM_DTR | TIOCM_RTS;
			ioctl(fd, TIOCMSET, &status);

			usleep(5000); // 5ms

			device = UARTDevice(
				fd,
				Move(name),
				speed
			);
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
				-1,
				"",
				0
			)
		{
		}

		UARTDevice(UARTDevice&& device)
			: fd(
				device.fd
			),
			name(
				Move(device.name)
			),
			speed(
				device.speed
			)
		{
			device.fd = -1;
		}

		UARTDevice(String&& name, UARTDeviceSpeed speed)
			: fd(
				-1
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
			return fd != -1;
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
			size_t bytesRead;

			if ((bytesRead = read(fd, lpBuffer, size)) == -1)
			{

				throw Exceptions::SystemException(
					"read"
				);
			}

			return bytesRead;
		}

		// @throw AL::Exceptions::Exception
		void Write(const void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "UARTDevice not open");

			if (write(fd, lpBuffer, size) == -1)
			{

				throw Exceptions::SystemException(
					"write"
				);
			}
		}

		void Close()
		{
			if (IsOpen())
			{
				close(fd);
				fd = -1;
			}
		}

		UARTDevice& operator = (UARTDevice&& device)
		{
			Close();

			fd = device.fd;
			device.fd = -1;

			name = Move(
				device.name
			);

			speed = device.speed;

			return *this;
		}

	private:
		// @return false on invalid speed
		static bool SpeedToBaud(speed_t& baud, UARTDeviceSpeed speed)
		{
#define AL_SPEED_TO_BAUD_CASE(__value__) case __value__: baud = B##__value__; return true

			switch (speed)
			{
				AL_SPEED_TO_BAUD_CASE(0);
				AL_SPEED_TO_BAUD_CASE(50);
				AL_SPEED_TO_BAUD_CASE(75);
				AL_SPEED_TO_BAUD_CASE(110);
				AL_SPEED_TO_BAUD_CASE(134);
				AL_SPEED_TO_BAUD_CASE(150);
				AL_SPEED_TO_BAUD_CASE(200);
				AL_SPEED_TO_BAUD_CASE(300);
				AL_SPEED_TO_BAUD_CASE(600);
				AL_SPEED_TO_BAUD_CASE(1200);
				AL_SPEED_TO_BAUD_CASE(1800);
				AL_SPEED_TO_BAUD_CASE(2400);
				AL_SPEED_TO_BAUD_CASE(4800);
				AL_SPEED_TO_BAUD_CASE(9600);
				AL_SPEED_TO_BAUD_CASE(19200);
				AL_SPEED_TO_BAUD_CASE(38400);
				AL_SPEED_TO_BAUD_CASE(57600);
				AL_SPEED_TO_BAUD_CASE(115200);
				AL_SPEED_TO_BAUD_CASE(230400);
				AL_SPEED_TO_BAUD_CASE(460800);
				AL_SPEED_TO_BAUD_CASE(500000);
				AL_SPEED_TO_BAUD_CASE(576000);
				AL_SPEED_TO_BAUD_CASE(921600);
				AL_SPEED_TO_BAUD_CASE(1000000);
				AL_SPEED_TO_BAUD_CASE(1152000);
				AL_SPEED_TO_BAUD_CASE(1500000);
				AL_SPEED_TO_BAUD_CASE(2000000);
				AL_SPEED_TO_BAUD_CASE(2500000);
				AL_SPEED_TO_BAUD_CASE(3000000);
				AL_SPEED_TO_BAUD_CASE(3500000);
				AL_SPEED_TO_BAUD_CASE(4000000);
			}

#undef AL_SPEED_TO_BAUD_CASE

			return false;
		}
	};
}
