#pragma once
#include "AL/Common.hpp"

namespace AL::Tasks
{
	class TaskThread;
	class TaskThreadPool;

	class Task
		: public Function<void()>
	{
	public:
		// @throw AL::Exceptions::Exception
		static void Execute(Task&& task)
		{
			try
			{
				OS::Thread::StartAndDetach(
					[task = Move(task)]()
					{
						task();
					}
				);
			}
			catch (Exceptions::Exception& exception)
			{

				throw Exceptions::Exception(
					Move(exception),
					"Error starting Task thread (OS::Thread)"
				);
			}
		}
		// @throw AL::Exceptions::Exception
		static void Execute(TaskThread& thread, Task&& task);
		// @throw AL::Exceptions::Exception
		static void Execute(TaskThreadPool& threadPool, Task&& task);

		using Function<void()>::Function;
	};
}
