#pragma once
#include "AL/Common.hpp"

#include "AL/OS/SystemException.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#include <sys/ioctl.h>

namespace AL::Hardware
{
	enum class UARTDeviceFlags : uint8
	{
		None         = 0x0,

		Parity       = 0x1,
		Parity_Odd   = Parity | 0x2,
		Parity_Even  = Parity | 0x4
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(UARTDeviceFlags);

	enum class UARTDeviceSpeeds : ::speed_t
	{
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
		
		Default      = Baud_9600
	};

	class UARTDevice
	{
		Bool                     isOpen = False;

		int                      fd;

		String                   path;
		BitMask<UARTDeviceFlags> flags;
		UARTDeviceSpeeds         speed;

		UARTDevice(const UARTDevice&) = delete;

	public:
		UARTDevice(UARTDevice&& device)
			: isOpen(
				device.isOpen
			),
			fd(
				device.fd
			),
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

		UARTDevice(String&& path, UARTDeviceSpeeds speed, UARTDeviceFlags flags)
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
		UARTDevice(const String& path, UARTDeviceSpeeds speed, UARTDeviceFlags flags)
			: UARTDevice(
				String(path),
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

			if ((fd = ::open(GetPath().GetCString(), O_RDWR | O_NOCTTY | O_NDELAY)) == -1)
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

//			if (GetFlags().IsSet(UARTDeviceFlags::Use2StopBits))
//			{
//
//				options.c_cflag |= CSTOPB;
//			}

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

			isOpen = True;
		}

		Void Close()
		{
			if (IsOpen())
			{
				::close(
					fd
				);

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
				int bytesRead;

				if ((bytesRead = ::read(fd, &reinterpret_cast<uint8*>(lpBuffer)[totalBytesRead], size - totalBytesRead)) == -1)
				{

					throw OS::SystemException(
						"read"
					);
				}

				totalBytesRead += bytesRead;
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
				int bytesWritten;

				if ((bytesWritten = ::write(fd, &reinterpret_cast<const uint8*>(lpBuffer)[totalBytesWritten], size - totalBytesWritten)) == -1)
				{

					throw OS::SystemException(
						"write"
					);
				}

				totalBytesWritten += bytesWritten;
			}
		}

		UARTDevice& operator = (UARTDevice&& device)
		{
			Close();

			isOpen = device.isOpen;
			device.isOpen = False;

			fd = device.fd;

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
