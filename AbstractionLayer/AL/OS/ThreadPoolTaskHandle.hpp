#pragma once
#include "AL/Common.hpp"

#include "ThreadPoolTask.hpp"

namespace AL::OS
{
	class ThreadPoolTaskHandle
	{
		bool isLocked = false;

		ThreadPoolTask* lpTask;
		Mutex           mutex;

	public:
		ThreadPoolTaskHandle()
			: lpTask(
				nullptr
			)
		{
		}

		explicit ThreadPoolTaskHandle(ThreadPoolTask& task)
			: lpTask(
				&task
			)
		{
			task.AddReference();
		}

		ThreadPoolTaskHandle(ThreadPoolTaskHandle&& handle)
			: lpTask(
				handle.lpTask
			)
		{
			handle.lpTask = nullptr;
		}

		ThreadPoolTaskHandle(const ThreadPoolTaskHandle& handle)
			: lpTask(
				handle.lpTask
			)
		{
			if (lpTask != nullptr)
			{

				lpTask->AddReference();
			}
		}

		virtual ~ThreadPoolTaskHandle()
		{
			Release();
		}

		bool IsLocked() const
		{
			return isLocked;
		}

		void Lock()
		{
			mutex.Lock();
			isLocked = true;
		}

		void Unlock()
		{
			isLocked = false;
			mutex.Unlock();
		}

		void Release()
		{
			if (lpTask != nullptr)
			{
				lpTask->RemoveReference();

				lpTask = nullptr;
			}
		}

		operator bool () const
		{
			return lpTask != nullptr;
		}

		ThreadPoolTask* operator -> () const
		{
			return lpTask;
		}

		auto& operator = (ThreadPoolTaskHandle&& handle)
		{
			Release();

			lpTask = handle.lpTask;
			handle.lpTask = nullptr;

			return *this;
		}

		auto& operator = (const ThreadPoolTaskHandle& handle)
		{
			Release();

			if ((lpTask = handle.lpTask) != nullptr)
			{

				lpTask->AddReference();
			}

			return *this;
		}

		bool operator == (const ThreadPoolTaskHandle& handle) const
		{
			return lpTask == handle.lpTask;
		}
		bool operator != (const ThreadPoolTaskHandle& handle) const
		{
			return !operator==(
				handle
			);
		}
	};
}
