#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <mutex>
#endif

namespace AL::OS
{
	class Mutex
	{
#if defined(AL_PLATFORM_LINUX)
		std::mutex mutex;
#elif defined(AL_PLATFORM_WINDOWS)
		CRITICAL_SECTION section;
#endif

	public:
		Mutex()
		{
#if defined(AL_PLATFORM_WINDOWS)
			InitializeCriticalSection(&section);
#endif
		}

		virtual ~Mutex()
		{
#if defined(AL_PLATFORM_WINDOWS)
			DeleteCriticalSection(&section);
#endif
		}

		void Lock()
		{
#if defined(AL_PLATFORM_LINUX)
			mutex.lock();
#elif defined(AL_PLATFORM_WINDOWS)
			EnterCriticalSection(&section);
#endif
		}

		void Unlock()
		{
#if defined(AL_PLATFORM_LINUX)
			mutex.unlock();
#elif defined(AL_PLATFORM_WINDOWS)
			LeaveCriticalSection(&section);
#endif
		}
	};

	class MutexGuard
	{
		Mutex* const lpMutex;

	public:
		explicit MutexGuard(Mutex& mutex)
			: lpMutex(
				&mutex
			)
		{
			lpMutex->Lock();
		}

		virtual ~MutexGuard()
		{
			lpMutex->Unlock();
		}
	};
}
