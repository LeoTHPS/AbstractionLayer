#pragma once
#include "AL/Common.hpp"

#include "TaskThread.hpp"

#include "AL/Collections/Array.hpp"

namespace AL::Tasks
{
	class TaskThreadPool
	{
		bool isRunning = false;
		bool isStopping = false;

		Collections::Array<TaskThread*> threads;

		TaskThreadPool(TaskThreadPool&&) = delete;
		TaskThreadPool(const TaskThreadPool&) = delete;

	public:
		TaskThreadPool()
		{
		}

		virtual ~TaskThreadPool()
		{
		}

		bool IsRunning() const
		{
			return isRunning;
		}

		// @throw AL::Exceptions::Exception
		void Start(size_t threadCount, const TaskThreadExceptionHandler& exceptionHandler)
		{
			AL_ASSERT(!IsRunning(), "TaskThreadPool already running");

			threads.SetSize(
				threadCount
			);

			for (size_t i = 0; i < threadCount; ++i)
			{
				threads[i] = new TaskThread();

				try
				{
					threads[i]->Start(
						TaskThreadExceptionHandler(
							[&exceptionHandler](Exceptions::Exception& _exception)
							{
								exceptionHandler(
									_exception
								);
							}
						)
					);
				}
				catch (Exceptions::Exception&)
				{
					if (i > 0)
					{
						while (--i > 0)
						{
							threads[i]->Stop();

							delete threads[i];
							threads[i] = nullptr;
						}

						threads[i]->Stop();

						delete threads[i];
						threads[i] = nullptr;
					}

					threads.Clear();

					throw;
				}
			}

			isRunning = true;
		}

		void Stop()
		{
			if (IsRunning())
			{
				if (!isStopping)
				{
					isStopping = true;

					for (auto& lpThread : threads)
					{
						lpThread->Stop();

						delete lpThread;
						lpThread = nullptr;
					}

					isRunning = false;
					isStopping = false;

					threads.Clear();
				}
				else
				{
					for (auto lpThread : threads)
					{
						lpThread->Join();
					}
				}
			}
		}

		// @throw AL::Exceptions::Exception
		// @return false if time elapsed and threads are still running
		bool Join(TimeSpan maxWaitTime = TimeSpan::Infinite)
		{
			OS::Timer timer;

			for (auto lpThread : threads)
			{
				if (lpThread && !lpThread->Join(maxWaitTime - timer.GetElapsed()))
				{

					return false;
				}
			}

			return true;
		}

		void Post(Task&& task)
		{
			TaskThread* lpSelectedThread = nullptr;

			for (auto lpThread : threads)
			{
				if (!lpSelectedThread || (lpThread && (lpThread->GetQueueSize() > lpSelectedThread->GetQueueSize())))
				{
					lpSelectedThread = lpThread;
				}
			}

			if (lpSelectedThread)
			{
				lpSelectedThread->Post(
					Move(task)
				);
			}
		}
	};
}
