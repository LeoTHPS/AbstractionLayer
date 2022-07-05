#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_PICO)
	#warning Platform not supported
#endif

#include "Thread.hpp"

#include "AL/Collections/Array.hpp"
#include "AL/Collections/Tuple.hpp"
#include "AL/Collections/MPSCQueue.hpp"

namespace AL::OS
{
	class ThreadPool
	{
		typedef Function<Void()>                   ThreadTask;
		typedef Collections::MPSCQueue<ThreadTask> ThreadTaskQueue;

		struct ThreadContext
		{
			OS::Thread      Thread;
			ThreadTaskQueue TaskQueue;
		};

		Bool isRunning  = False;
		Bool isStopping = False;

		Collections::Array<ThreadContext> threads;

	public:
		explicit ThreadPool(size_t count)
			: threads(
				count
			)
		{
		}

		virtual ~ThreadPool()
		{
			if (IsRunning())
			{

				Stop();
			}
		}

		Bool IsRunning() const
		{
			return isRunning;
		}

		auto GetCount() const
		{
			return threads.GetSize();
		}

		// @throw AL::Exception
		Void Start()
		{
			AL_ASSERT(
				!IsRunning(),
				"ThreadPool already running"
			);

			for (size_t i = 0; i < threads.GetSize(); ++i)
			{
				auto& context = threads[i];

				Function<Void()> threadStart(
					[this, &context]()
					{
						Thread_Main(
							context
						);
					}
				);

				try
				{
					context.Thread.Start(
						Move(threadStart)
					);
				}
				catch (Exception& exception)
				{
					StopThreads();

					throw Exception(
						Move(exception),
						"Error starting Thread"
					);
				}
			}

			isRunning = True;
		}

		Void Stop()
		{
			if (IsRunning())
			{
				StopThreads();

				isRunning = False;
			}
		}

		template<typename F, typename ... TArgs>
		Void Post(F&& function, TArgs ... args)
		{
			AL_ASSERT(
				IsRunning(),
				"ThreadPool not running"
			);

			AL_ASSERT(
				!isStopping,
				"ThreadPool is stopping"
			);

			ThreadContext* lpThreadContext = &threads[0];
			ThreadContext* lpSelectedThreadContext = nullptr;

			for (size_t i = 0; i < threads.GetSize(); ++i, ++lpThreadContext)
			{
				if (lpThreadContext->TaskQueue.GetSize() == 0)
				{
					lpSelectedThreadContext = lpThreadContext;

					break;
				}
				else if (lpSelectedThreadContext == nullptr)
				{
					lpSelectedThreadContext = lpThreadContext;

					if (lpThreadContext->TaskQueue.GetSize() == 0)
					{

						break;
					}
				}
				else if (lpThreadContext->TaskQueue.GetSize() < lpSelectedThreadContext->TaskQueue.GetSize())
				{

					lpSelectedThreadContext = lpThreadContext;
				}
			}

			ThreadTask task(
				[f = Move(function), f_args = Collections::Tuple<TArgs ...>(Forward<TArgs>(args) ...)]()
				{
					f_args.Invoke(
						f
					);
				}
			);

			lpSelectedThreadContext->TaskQueue.Enqueue(
				Move(task)
			);
		}

	private:
		Bool IsAnyThreadRunning() const
		{
			auto lpContext = &threads[0];

			for (size_t i = 0; i < threads.GetSize(); ++i, ++lpContext)
			{
				if (lpContext->Thread.IsRunning())
				{

					return True;
				}
			}

			return False;
		}

		Void StopThreads()
		{
			isStopping = True;

			while (IsAnyThreadRunning())
			{
				Sleep(
					TimeSpan::FromMilliseconds(10)
				);
			}

			isStopping = False;
		}

		Void Thread_Main(ThreadContext& context)
		{
			ThreadTask task;

			do
			{
				while (context.TaskQueue.Dequeue(task))
				{

					task();
				}

				if (!isStopping)
				{

					Sleep(
						TimeSpan::FromMilliseconds(10)
					);
				}
			} while (!isStopping || (context.TaskQueue.GetSize() != 0));
		}
	};
}
