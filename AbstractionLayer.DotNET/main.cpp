#include "AL/DotNET/Common.hpp"

#include "AL/DotNET/Algorithms/CRC.hpp"
#include "AL/DotNET/Algorithms/FNV.hpp"
#include "AL/DotNET/Algorithms/Isaac.hpp"
#include "AL/DotNET/Algorithms/Sha1.hpp"

#include "AL/DotNET/Bluetooth/Radio.hpp"
#include "AL/DotNET/Bluetooth/Device.hpp"

#include "AL/DotNET/Collections/ByteBuffer.hpp"

#include "AL/DotNET/Exceptions/Exception.hpp"
#include "AL/DotNET/Exceptions/NotImplementedException.hpp"

#include "AL/DotNET/FileSystem/INIFile.hpp"
#include "AL/DotNET/FileSystem/LogFile.hpp"

#include "AL/DotNET/GPIO/UARTDevice.hpp"
#include "AL/DotNET/GPIO/UARTDeviceSpeeds.hpp"

#include "AL/DotNET/GPIO/Devices/BN_180.hpp"

#include "AL/DotNET/OS/Timer.hpp"
#include "AL/DotNET/OS/Detour.hpp"
#include "AL/DotNET/OS/System.hpp"
#include "AL/DotNET/OS/Library.hpp"
#include "AL/DotNET/OS/Process.hpp"
#include "AL/DotNET/OS/Environment.hpp"

#if defined(AL_PLATFORM_WINDOWS)
	#include "AL/DotNET/OS/ntdll.hpp"
#endif
