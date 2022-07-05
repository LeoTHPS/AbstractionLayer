#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#warning Platform not supported
#endif

#include "AsyncFunction.hpp"

namespace AL::OS::Windows
{
	template<typename T_KEY, typename T_PARAM>
	class CompletionWorker;

	template<typename T_KEY, typename T_PARAM>
	class CompletionPort
	{
		static_assert(
			Is_Enum<T_KEY>::Value || Is_Integer<T_KEY>::Value,
			"T_KEY must be enum or integer"
		);

		static_assert(
			sizeof(T_KEY) <= sizeof(::ULONG_PTR),
			"T_KEY size must be less than or equal to ULONG_PTR"
		);

		Bool isCreated = False;

		::HANDLE handle = NULL;

		CompletionPort(CompletionPort&&) = delete;
		CompletionPort(const CompletionPort&) = delete;

	public:
		typedef T_KEY                        Key;
		typedef T_PARAM                      Param;

		typedef CompletionWorker<Key, Param> Worker;

		CompletionPort()
		{
		}

		virtual ~CompletionPort()
		{
			Destroy();
		}

		Bool IsCreated() const
		{
			return isCreated;
		}

		auto GetHandle() const
		{
			return handle;
		}

		// @throw AL::Exception
		Void Create(size_t maxConcurrentWorkers)
		{
			AL_ASSERT(
				!IsCreated(),
				"CompletionPort already created"
			);

			if (!(handle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, static_cast<::DWORD>(maxConcurrentWorkers))))
			{

				throw SystemException(
					"CreateIoCompletionPort"
				);
			}
		}

		Void Destroy()
		{
			if (IsCreated())
			{
				::CloseHandle(
					GetHandle()
				);

				isCreated = False;
			}
		}

		// @throw AL::Exception
		Void Associate(Key key, ::HANDLE handle)
		{
			AL_ASSERT(
				IsCreated(),
				"CompletionPort not created"
			);

			if (!::CreateIoCompletionPort(handle, GetHandle(), static_cast<::ULONG_PTR>(key), 0))
			{

				throw SystemException(
					"CreateIoCompletionPort"
				);
			}
		}

		// @throw AL::Exception
		Void Post(Key key, Param* lpParam, size_t bytesTransferred)
		{
			AL_ASSERT(
				IsCreated(),
				"CompletionPort not created"
			);

			if (!::PostQueuedCompletionStatus(GetHandle(), static_cast<::DWORD>(bytesTransferred), static_cast<::ULONG_PTR>(key), reinterpret_cast<::LPOVERLAPPED>(lpParam)))
			{

				throw SystemException(
					"PostQueuedCompletionStatus"
				);
			}
		}
	};

	template<typename T_KEY, typename T_PARAM>
	struct CompletionWork
	{
		T_KEY    Key;
		T_PARAM* lpParam;
		size_t   BytesTransferred;
	};

	template<typename T_KEY, typename T_PARAM>
	class CompletionWorker
	{
		CompletionPort<T_KEY, T_PARAM>* const lpPort;

		Bool isReading = False;
		::HANDLE hReadThread;

		Bool asyncWakePosted = False;
		AsyncFunction<Void()> asyncWake;

		CompletionWorker(CompletionWorker&&) = delete;
		CompletionWorker(const CompletionWorker&) = delete;

	public:
		typedef T_KEY                      Key;
		typedef T_PARAM                    Param;

		typedef CompletionPort<Key, Param> Port;
		typedef CompletionWork<Key, Param> Work;

		explicit CompletionWorker(Port& port)
			: lpPort(
				&port
			),
			asyncWake(
				[this]()
				{
					asyncWakePosted = False;
				}
			)
		{
		}

		Bool IsReading() const
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
		Void Cancel()
		{
			if (IsReading() && !asyncWakePosted)
			{
				asyncWakePosted = True;

				asyncWake(
					hReadThread
				);
			}
		}

		// @throw AL::Exception
		// @return number of completions read
		template<size_t S>
		size_t Read(Work(&queue)[S], TimeSpan maxWaitTime = TimeSpan::Infinite)
		{
			AL_ASSERT(
				GetPort().IsCreated(),
				"CompletionPort not created"
			);

			AL_ASSERT(
				!IsReading(),
				"CompletionWorker already reading"
			);

			hReadThread = GetCurrentThreadHandle();

			isReading = True;

			::ULONG entriesRead = 0;

			::OVERLAPPED_ENTRY entries[S];

			if (!::GetQueuedCompletionStatusEx(GetPort().GetHandle(), &entries[0], static_cast<::ULONG>(S), &entriesRead, static_cast<::DWORD>(maxWaitTime.ToMilliseconds()), TRUE))
			{
				switch (auto errorCode = GetLastError())
				{
					case WAIT_TIMEOUT:
					case WAIT_IO_COMPLETION:
						entriesRead = 0;
						break;

					default:
					{
						isReading = False;

						::CloseHandle(
							hReadThread
						);

						throw SystemException(
							"GetQueuedCompletionStatusEx",
							errorCode
						);
					}
				}
			}

			isReading = False;

			::CloseHandle(
				hReadThread
			);

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
		static ::HANDLE GetCurrentThreadHandle()
		{
			::HANDLE hThread;

			if (!::DuplicateHandle(::GetCurrentProcess(), ::GetCurrentThread(), ::GetCurrentProcess(), &hThread, 0, FALSE, DUPLICATE_SAME_ACCESS))
			{

				throw SystemException(
					"DuplicateHandle"
				);
			}

			return hThread;
		}
	};
}
