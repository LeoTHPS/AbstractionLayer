#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

namespace AL::OS
{
	template<typename KEY, typename PARAM>
	class CompletionWorker;

	template<typename KEY, typename PARAM>
	class CompletionPort
	{
		static_assert(
			Is_Enum<KEY>::Value || Is_Integer<KEY>::Value,
			"KEY must be enum or integer"
		);

		static_assert(
			sizeof(KEY) <= sizeof(ULONG_PTR),
			"KEY size must be less than or equal to ULONG_PTR"
		);

		HANDLE handle = NULL;
		
		CompletionPort(CompletionPort&&) = delete;
		CompletionPort(const CompletionPort&) = delete;

	public:
		typedef KEY   Key;
		typedef PARAM Param;

		typedef CompletionWorker<Key, Param> Worker;

		CompletionPort()
		{
		}

		virtual ~CompletionPort()
		{
			Destroy();
		}
		
		bool IsCreated() const
		{
			return handle != NULL;
		}

		auto GetHandle() const
		{
			return handle;
		}

		// @throw AL::Exception
		void Create(size_t maxConcurrentWorkers)
		{
			AL_ASSERT(!IsCreated(), "CompletionPort already created");

			if (!(handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, static_cast<DWORD>(maxConcurrentWorkers))))
			{

				throw Exceptions::SystemException(
					"CreateIoCompletionPort"
				);
			}
		}

		void Destroy()
		{
			if (IsCreated())
			{
				CloseHandle(
					GetHandle()
				);

				handle = NULL;
			}
		}

		// @throw AL::Exception
		void Associate(Key key, HANDLE handle)
		{
			AL_ASSERT(IsCreated(), "CompletionPort not created");

			if (!CreateIoCompletionPort(handle, GetHandle(), static_cast<ULONG_PTR>(key), 0))
			{

				throw Exceptions::SystemException(
					"CreateIoCompletionPort"
				);
			}
		}

		// @throw AL::Exception
		void Post(Key key, Param* lpParam, size_t bytesTransferred)
		{
			if (!IsCreated())
			{

				throw Exceptions::Exception(
					"CompletionPort not created"
				);
			}

			if (!PostQueuedCompletionStatus(GetHandle(), static_cast<DWORD>(bytesTransferred), static_cast<ULONG_PTR>(key), reinterpret_cast<LPOVERLAPPED>(lpParam)))
			{

				throw Exceptions::SystemException(
					"PostQueuedCompletionStatus"
				);
			}
		}
	};

	template<typename KEY, typename PARAM>
	struct CompletionWork
	{
		KEY    Key;
		PARAM* lpParam;
		size_t BytesTransferred;
	};

	template<typename KEY, typename PARAM>
	class CompletionWorker
	{
		CompletionPort<KEY, PARAM>* const lpPort;

		bool isReading = false;
		HANDLE hReadThread;

		bool asyncWakePosted = false;
		AsyncFunction<void()> asyncWake;

		CompletionWorker(CompletionWorker&&) = delete;
		CompletionWorker(const CompletionWorker&) = delete;

	public:
		typedef KEY Key;
		typedef PARAM Param;

		typedef CompletionPort<Key, Param> Port;
		typedef CompletionWork<Key, Param> Work;

		explicit CompletionWorker(Port& port)
			: lpPort(
				&port
			),
			asyncWake(
				[this]()
				{
					asyncWakePosted = false;
				}
			)
		{
		}

		bool IsReading() const
		{
			return isReading;
		}

		auto& GetPort()
		{
			return *lpPort;
		}
		auto& GetPort() const
		{
			return *lpPort;
		}

		// @throw AL::Exception
		void Cancel()
		{
			if (IsReading() && !asyncWakePosted)
			{
				asyncWakePosted = true;

				asyncWake(hReadThread);
			}
		}

		// @throw AL::Exception
		// @return number of completions read
		template<size_t S>
		size_t Read(Work(&queue)[S], TimeSpan maxWaitTime = TimeSpan::Infinite)
		{
			AL_ASSERT(GetPort().IsCreated(), "CompletionPort not created");

			AL_ASSERT(!IsReading(), "CompletionWorker already reading");

			hReadThread = GetCurrentThreadHandle();

			isReading = true;

			ULONG entriesRead = 0;

			OVERLAPPED_ENTRY entries[S];

			if (!GetQueuedCompletionStatusEx(GetPort().GetHandle(), &entries[0], static_cast<ULONG>(S), &entriesRead, static_cast<DWORD>(maxWaitTime.ToMilliseconds()), TRUE))
			{
				switch (auto errorCode = GetLastError())
				{
					case WAIT_TIMEOUT:
					case WAIT_IO_COMPLETION:
						entriesRead = 0;
						break;

					default:
					{
						isReading = false;

						CloseHandle(hReadThread);

						throw Exceptions::SystemException(
							"GetQueuedCompletionStatusEx",
							errorCode
						);
					}
				}
			}

			isReading = false;

			CloseHandle(hReadThread);

			for (size_t i = 0; i < entriesRead; ++i)
			{
				auto lpWork = &queue[i];
				auto lpEntry = &entries[i];
				
				lpWork->Key = static_cast<Key>(
					lpEntry->lpCompletionKey
				);
				lpWork->lpParam = reinterpret_cast<Param*>(
					lpEntry->lpOverlapped
				);
				lpWork->BytesTransferred = static_cast<size_t>(
					lpEntry->dwNumberOfBytesTransferred
				);
			}

			return static_cast<size_t>(
				entriesRead
			);
		}

	private:
		// @throw AL::Exception
		static HANDLE GetCurrentThreadHandle()
		{
			HANDLE hThread;

			if (!DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &hThread, 0, FALSE, DUPLICATE_SAME_ACCESS))
			{

				throw Exceptions::SystemException(
					"DuplicateHandle"
				);
			}

			return hThread;
		}
	};
}
