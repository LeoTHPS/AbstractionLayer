#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <mutex>
#endif

namespace AL::OS
{
	class Mutex
	{
		Mutex(Mutex&&) = delete;
		Mutex(const Mutex&) = delete;

	public:
#if defined(AL_PLATFORM_LINUX)
		typedef ::std::mutex       Type;
#elif defined(AL_PLATFORM_WINDOWS)
		typedef ::CRITICAL_SECTION Type;
#endif

		Mutex()
		{
#if defined(AL_PLATFORM_WINDOWS)
			::InitializeCriticalSection(
				&mutex
			);
#endif
		}

		virtual ~Mutex()
		{
#if defined(AL_PLATFORM_WINDOWS)
			::DeleteCriticalSection(
				&mutex
			);
#endif
		}

		Void Lock()
		{
#if defined(AL_PLATFORM_LINUX)
			mutex.lock();
#elif defined(AL_PLATFORM_WINDOWS)
			::EnterCriticalSection(
				&mutex
			);
#endif
		}

		Void Unlock()
		{
#if defined(AL_PLATFORM_LINUX)
			mutex.unlock();
#elif defined(AL_PLATFORM_WINDOWS)
			::LeaveCriticalSection(
				&mutex
			);
#endif
		}

		operator Type& ()
		{
			return mutex;
		}
		operator const Type& () const
		{
			return mutex;
		}

	private:
		Type mutex;
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
