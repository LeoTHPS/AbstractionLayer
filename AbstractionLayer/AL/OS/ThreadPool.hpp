#pragma once
#include "AL/Common.hpp"

#include "ThreadPoolTask.hpp"
#include "ThreadPoolTaskHandle.hpp"

#include "AL/Collections/Array.hpp"
#include "AL/Collections/Queue.hpp"
#include "AL/Collections/Dictionary.hpp"

namespace AL::OS
{
	class ThreadPool
	{
		class Worker
		{
			typedef Collections::Queue<ThreadPoolTaskHandle>     TaskQueue;
			typedef Collections::Dictionary<TimeSpan, TaskQueue> TaskSchedule;

			bool isStopping = false;

			ThreadPool* const lpPool;
			Thread            thread;

			TaskSchedule   tasks;
			size_t         taskCount;
			Timer          tasksTimer;
			Mutex          tasksMutex;
			MutexCondition tasksCondition;

			Worker(Worker&&) = delete;
			Worker(const Worker&) = delete;

		public:
			explicit Worker(ThreadPool& pool)
				: lpPool(
					&pool
				),
				taskCount(
					0
				)
			{
			}

			virtual ~Worker()
			{
			}

			bool IsRunning() const
			{
				return thread.IsRunning();
			}

			// @throw AL::Exceptions::Exception
			void Start()
			{
				AL_ASSERT(!IsRunning(), "Worker already running");

				thread.Start(
					&Worker::Main,
					this
				);
			}

			void Stop()
			{
				if (IsRunning() && !isStopping)
				{
					isStopping = true;
				}
			}

			void Join()
			{
				while (IsRunning())
				{
					Sleep(
						TimeSpan::FromMilliseconds(10)
					);
				}
			}

			template<typename F, typename ... TArgs>
			ThreadPoolTaskHandle Post(F&& callback, TArgs ... args)
			{
				AL_ASSERT(IsRunning(), "Worker not running");

				auto lpTask = new ThreadPoolTask(
					Move(callback),
					Forward<TArgs>(args) ...
				);

				auto hTask = Tasks_Enqueue(
					0,
					lpTask
				);

				tasksCondition.WakeOne();

				return hTask;
			}

			template<typename F, typename ... TArgs>
			ThreadPoolTaskHandle Schedule(TimeSpan delay, F&& callback, TArgs ... args)
			{
				AL_ASSERT(IsRunning(), "Worker not running");

				auto lpTask = new ThreadPoolTask(
					Move(callback),
					Forward<TArgs>(args) ...
				);

				auto hTask = Tasks_Enqueue(
					delay,
					lpTask
				);

				tasksCondition.WakeOne();

				return hTask;
			}

			int ComparePriority(const Worker& worker) const
			{
				auto taskCount = tasks.GetSize();
				auto workerTaskCount = worker.tasks.GetSize();

				if (taskCount > workerTaskCount)
				{

					return -1;
				}

				if (taskCount < workerTaskCount)
				{

					return 1;
				}

				return 0;
			}

		private:
			void Main()
			{
				ThreadPoolTaskHandle hTask;

				do
				{
					while (Tasks_Dequeue(hTask))
					{
						hTask.Lock();

						if (hTask && hTask->IsPending() && !hTask->IsCancelled())
						{
							try
							{
								hTask->Execute();
							}
							catch (const Exceptions::Exception& exception)
							{
								Debug::WriteLine(
									exception.GetMessage()
								);

								if (auto lpInnerException = exception.GetInnerException())
								{
									do
									{
										Debug::WriteLine(
											lpInnerException->GetMessage()
										);
									} while ((lpInnerException = lpInnerException->GetInnerException()) != nullptr);
								}
							}
						}

						hTask.Unlock();
					}

					tasksCondition.Sleep(
						TimeSpan::FromMilliseconds(100)
					);
				} while (!isStopping || taskCount);

				isStopping = false;
			}

			bool Tasks_Dequeue(ThreadPoolTaskHandle& hTask)
			{
				MutexGuard lock(
					tasksMutex
				);

				for (auto it = tasks.begin(); it != tasks.end(); ++it)
				{
					if (it->first <= tasksTimer.GetElapsed())
					{
						it->second.Dequeue(
							hTask
						);

						--taskCount;

						if (it->second.GetSize() == 0)
						{
							tasks.Erase(
								it
							);
						}

						return true;
					}
				}

				return false;
			}

			ThreadPoolTaskHandle Tasks_Enqueue(TimeSpan delay, ThreadPoolTask* lpTask)
			{
				{
					MutexGuard lock(
						tasksMutex
					);

					tasks[tasksTimer.GetElapsed() + delay].Emplace<ThreadPoolTask&>(
						*lpTask
					);

					++taskCount;
				}

				return ThreadPoolTaskHandle(
					*lpTask
				);
			}
		};

		bool isRunning = false;

		Collections::Array<Worker*> workers;

		ThreadPool(ThreadPool&&) = delete;
		ThreadPool(const ThreadPool&) = delete;

	public:
		ThreadPool()
		{
		}

		virtual ~ThreadPool()
		{
		}

		bool IsRunning() const
		{
			return isRunning;
		}

		// @throw AL::Exceptions::Exception
		void Start(size_t threadCount)
		{
			AL_ASSERT(!IsRunning(), "ThreadPool already running");

			workers.SetCapacity(
				threadCount
			);

			for (size_t i = 0; i < threadCount; ++i)
			{
				auto lpWorker = new Worker(
					*this
				);

				try
				{
					lpWorker->Start();
				}
				catch (Exceptions::Exception& exception)
				{
					delete lpWorker;

					for (auto it = workers.rbegin(); it != workers.rend(); )
					{
						(*it)->Stop();
						(*it)->Join();

						delete *it;
						
						--it;

						workers.PopBack();
					}

					throw Exceptions::Exception(
						Move(exception),
						"Error starting Worker"
					);
				}

				workers.PushBack(
					lpWorker
				);
			}

			isRunning = true;
		}

		void Stop()
		{
			if (IsRunning())
			{
				for (auto lpWorker : workers)
				{
					lpWorker->Stop();
				}

				for (auto it = workers.begin(); it != workers.end(); )
				{
					(*it)->Join();

					delete *it;

					workers.Erase(
						it++
					);
				}

				isRunning = false;
			}
		}

		// @throw AL::Exceptions::Exception
		template<typename F, typename ... TArgs>
		ThreadPoolTaskHandle Post(F&& function, TArgs ... args)
		{
			AL_ASSERT(IsRunning(), "ThreadPool not running");

			ThreadPoolTaskHandle hTask;

			if (auto lpWorker = GetHighestPriorityWorker())
			{

				hTask = lpWorker->Post(
					Move(function),
					Forward<TArgs>(args) ...
				);
			}

			return hTask;
		}

		// @throw AL::Exceptions::Exception
		template<typename F, typename ... TArgs>
		ThreadPoolTaskHandle Schedule(TimeSpan delay, F&& function, TArgs ... args)
		{
			AL_ASSERT(IsRunning(), "ThreadPool not running");

			ThreadPoolTaskHandle hTask;

			if (auto lpWorker = GetHighestPriorityWorker())
			{

				hTask = lpWorker->Schedule(
					delay,
					Move(function),
					Forward<TArgs>(args) ...
				);
			}

			return hTask;
		}

	private:
		Worker* GetHighestPriorityWorker() const
		{
			Worker* _lpWorker = nullptr;

			for (auto lpWorker : workers)
			{
				if (!_lpWorker || (lpWorker->ComparePriority(*_lpWorker) > 0))
				{
					_lpWorker = lpWorker;
				}
			}

			return _lpWorker;
		}
	};
}
