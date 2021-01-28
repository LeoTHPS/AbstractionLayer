#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <termios.h> // speed_t
#endif

namespace AL::GPIO
{
	enum class UARTDeviceSpeeds
#if defined(AL_PLATFORM_LINUX)
		: speed_t
#elif defined(AL_PLATFORM_WINDOWS)
		: DWORD
#endif
	{
		Baud_0             = 0,
#if defined(AL_PLATFORM_LINUX)
		Baud_50            = B50,
		Baud_75            = B75,
		Baud_110           = B110,
		Baud_134           = B134,
		Baud_150           = B150,
		Baud_200           = B200,
		Baud_300           = B300,
		Baud_600           = B600,
		Baud_1200          = B1200,
		Baud_1800          = B1800,
		Baud_2400          = B2400,
		Baud_4800          = B4800,
		Baud_9600          = B9600,
		Baud_19200         = B19200,
		Baud_38400         = B38400,
		Baud_57600         = B57600,
		Baud_115200        = B115200,
		Baud_230400        = B230400,
		Baud_460800        = B460800,
		Baud_500000        = B500000,
		Baud_576000        = B576000,
		Baud_921600        = B921600,
		Baud_1000000       = B1000000,
		Baud_1152000       = B1152000,
		Baud_1500000       = B1500000,
		Baud_2000000       = B2000000,
		Baud_2500000       = B2500000,
		Baud_3000000       = B3000000,
		Baud_3500000       = B3500000,
		Baud_4000000       = B4000000,
#elif defined(AL_PLATFORM_WINDOWS)
		Baud_110           = CBR_110,
		Baud_300           = CBR_300,
		Baud_600           = CBR_600,
		Baud_1200          = CBR_1200,
		Baud_2400          = CBR_2400,
		Baud_4800          = CBR_4800,
		Baud_9600          = CBR_9600,
		Baud_14400         = CBR_14400,
		Baud_19200         = CBR_19200,
		Baud_38400         = CBR_38400,
		Baud_56000         = CBR_56000,
		Baud_57600         = CBR_57600,
		Baud_115200        = CBR_115200,
		Baud_128000        = CBR_128000,
		Baud_256000        = CBR_256000,
#endif

		Common_Baud_110    = Baud_110,
		Common_Baud_300    = Baud_300,
		Common_Baud_600    = Baud_600,
		Common_Baud_1200   = Baud_1200,
		Common_Baud_2400   = Baud_2400,
		Common_Baud_4800   = Baud_4800,
		Common_Baud_9600   = Baud_9600,
		Common_Baud_19200  = Baud_19200,
		Common_Baud_38400  = Baud_38400,
		Common_Baud_57600  = Baud_57600,
		Common_Baud_115200 = Baud_115200
	};
}
