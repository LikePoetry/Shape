#pragma once

struct JobDispatchArgs
{
	uint32_t jobIndex;
	uint32_t groupID;
	uint32_t groupIndex;
	bool isFirstJobInGroup;
	bool isLastJobInGroup;
	void* sharedmemory;
};

namespace Shape
{
	namespace System
	{
		namespace JobSystem
		{
			void OnInit(uint32_t maxThreadCount = ~0u);

			uint32_t GetThreadCount();

			struct Context
			{
				std::atomic<uint32_t> counter{ 0 };
			};

			// Add a job to execute asynchronously. Any idle thread will execute this job.
			void Execute(Context& ctx, const std::function<void(JobDispatchArgs)>& task);

			// Divide a job onto multiple jobs and execute in parallel.
			//	jobCount	: how many jobs to generate for this task.
			//	groupSize	: how many jobs to execute per thread. Jobs inside a group execute serially. It might be worth to increase for small jobs
			//	func		: receives a JobDispatchArgs as parameter
			void Dispatch(Context& ctx, uint32_t jobCount, uint32_t groupSize, const std::function<void(JobDispatchArgs)>& task, size_t sharedmemory_size = 0);
		
			uint32_t DispatchGroupCount(uint32_t jobCount, uint32_t groupSize);

			// Check if any threads are working currently or not
			bool IsBusy(const Context& ctx);

			// Wait until all threads become idle
			void Wait(const Context& ctx);
		}
	}
}