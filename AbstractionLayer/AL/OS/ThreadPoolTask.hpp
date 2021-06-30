#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/Tuple.hpp"

namespace AL::OS
{
	class ThreadPool;

	class ThreadPoolTask
	{
		friend ThreadPool;

		volatile bool isPending = true;
		volatile bool isCancelled = false;

		Function<void()> function;

		size_t referenceCount = 1;

		ThreadPoolTask(ThreadPoolTask&&) = delete;
		ThreadPoolTask(const ThreadPoolTask&) = delete;

		template<typename F, typename ... TArgs>
		ThreadPoolTask(F&& callback, TArgs ... args)
			: function(
				[callback = Move(callback), args = Collections::Tuple<TArgs ...>(Forward<TArgs>(args) ...)]()
				{
					args.Invoke(
						callback
					);
				}
			)
		{
		}

	public:
		virtual ~ThreadPoolTask()
		{
		}

		bool IsPending() const
		{
			return isPending;
		}

		bool IsCancelled() const
		{
			return isCancelled;
		}

		auto GetReferenceCount() const
		{
			return referenceCount;
		}

		void Cancel()
		{
			if (IsPending())
			{
				isCancelled = true;
			}
		}

		void Execute()
		{
			function();

			isPending = false;
		}

		void AddReference()
		{
			++referenceCount;
		}

		void RemoveReference()
		{
			if (!--referenceCount)
			{
				delete this;
			}
		}
	};
}
