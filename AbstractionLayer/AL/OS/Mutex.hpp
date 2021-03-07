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

		Mutex(Mutex&&) = delete;
		Mutex(const Mutex&) = delete;

	public:
		Mutex()
		{
#if defined(AL_PLATFORM_WINDOWS)
			InitializeCriticalSection(
				&section
			);
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

#if defined(AL_PLATFORM_LINUX)
		operator std::mutex& ()
		{
			return mutex;
		}
		operator const std::mutex& () const
		{
			return mutex;
		}
#elif defined(AL_PLATFORM_WINDOWS)
		operator CRITICAL_SECTION& ()
		{
			return section;
		}
		operator const CRITICAL_SECTION& () const
		{
			return section;
		}
#endif
	};

	class MutexGuard
	{
		Mutex* const lpMutex;

		MutexGuard(MutexGuard&&) = delete;
		MutexGuard(const MutexGuard&) = delete;

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

		auto& GetMutex()
		{
			return *lpMutex;
		}
		auto& GetMutex() const
		{
			return *lpMutex;
		}
	};
}
