#pragma once
#include "AL/Common.hpp"

#include "Task.hpp"

#include "AL/OS/Thread.hpp"

#include "AL/Collections/MPSCQueue.hpp"

namespace AL::Tasks
{
	typedef Function<void(Exceptions::Exception& exception)> TaskThreadExceptionHandler;
	
	class TaskThread
	{
		volatile bool doStop = false;

		OS::Thread thread;

		Collections::MPSCQueue<Task> queue;
		TaskThreadExceptionHandler exceptionHandler;

		TaskThread(TaskThread&&) = delete;
		TaskThread(const TaskThread&) = delete;

	public:
		TaskThread()
		{
		}

		virtual ~TaskThread()
		{
		}

		bool IsRunning() const
		{
			return thread.IsRunning();
		}

		auto GetQueueSize() const
		{
			return queue.GetSize();
		}

		// @throw AL::Exceptions::Exception
		void Start(TaskThreadExceptionHandler&& exceptionHandler)
		{
			AL_ASSERT(!IsRunning(), "TaskThread already running");

			this->exceptionHandler = Move(
				exceptionHandler
			);

			thread.Start(
				[this]()
				{
					Task task;

					do
					{
						while (queue.Dequeue(task))
						{
							try
							{
								task();
							}
							catch (Exceptions::Exception& exception)
							{

								this->exceptionHandler(exception);
							}
						}

						Sleep(
							TimeSpan::FromMilliseconds(100)
						);
					} while (!doStop || queue.GetSize());

					doStop = false;
				}
			);
		}

		void Stop()
		{
			if (IsRunning())
			{
				doStop = true;

				thread.Join();
			}
		}

		void Post(Task&& task)
		{
			AL_ASSERT(IsRunning(), "TaskThread not running");

			queue.Enqueue(
				Move(task)
			);
		}

		// @throw AL::Exceptions::Exception
		// @return false if time elapsed and thread is still running
		bool Join(TimeSpan maxWaitTime = TimeSpan::Infinite)
		{
			AL_ASSERT(IsRunning(), "TaskThread not running");

			return thread.Join(
				maxWaitTime
			);
		}
	};
}

// @throw AL::Exceptions::Exception
inline void AL::Tasks::Task::Execute(TaskThread& thread, Task&& task)
{
	thread.Post(
		Move(task)
	);
}
