#include "hzpch.h"
#include "JobSystem.h"
#include "Log.h"
#include "Profiler.h"
#include "Shape/Maths/Maths.h"

#include <atomic>
#include <thread>
#include <condition_variable>
#include <deque>

#define NOMINMAX
#include <Windows.h>
#include <comdef.h>

namespace Shape
{
    namespace System
    {
        class SpinLock
        {
            std::atomic_flag locked = ATOMIC_FLAG_INIT;

        public:
            void lock()
            {
                while (locked.test_and_set(std::memory_order_acquire))
                {
                    // Continue.
                }
            }
            void unlock()
            {
                locked.clear(std::memory_order_release);
            }
        };

        namespace JobSystem
        {
            struct Job
            {
                Context* ctx;
                std::function<void(JobDispatchArgs)> task;
                uint32_t groupID;
                uint32_t groupJobOffset;
                uint32_t groupJobEnd;
                uint32_t sharedmemory_size;
            };

            struct JobQueue
            {
                std::atomic_bool processing{ false };
                std::deque<Job> queue;
                SpinLock locker;

                inline void push_back(const Job& item)
                {
                    std::scoped_lock lock(locker);
                    queue.push_back(item);
                }

                inline bool pop_front(Job& item)
                {
                    std::scoped_lock lock(locker);
                    if (queue.empty())
                    {
                        processing.store(false);
                        return false;
                    }
                    item = std::move(queue.front());
                    queue.pop_front();
                    processing.store(true);
                    return true;
                }
            };

            struct WorkerState
            {
                std::atomic_bool alive{ true };
                std::condition_variable wakeCondition;
                std::mutex wakeMutex;
            };

            // This structure is responsible to stop worker thread loops.
            //    Once this is destroyed, worker threads will be woken up and end their loops.
            struct InternalState
            {
                uint32_t numCores = 0;
                uint32_t numThreads = 0;
                std::unique_ptr<JobQueue[]> jobQueuePerThread;
                std::shared_ptr<WorkerState> worker_state = std::make_shared<WorkerState>(); // kept alive by both threads and internal_state
                std::atomic<uint32_t> nextQueue{ 0 };
                ~InternalState()
                {
                    worker_state->alive.store(false);         // indicate that new jobs cannot be started from this point
                    worker_state->wakeCondition.notify_all(); // wakes up sleeping worker threads
                    // wait until all currently running jobs finish:
                    for (uint32_t i = 0; i < numThreads; ++i)
                    {
                        while (jobQueuePerThread[i].processing.load())
                        {
                            std::this_thread::yield();
                        }
                    }
                }
            } static internal_state;

            // Start working on a job queue
            //    After the job queue is finished, it can switch to an other queue and steal jobs from there
            inline void work(uint32_t startingQueue)
            {
                Job job;
                for (uint32_t i = 0; i < internal_state.numThreads; ++i)
                {
                    JobQueue& job_queue = internal_state.jobQueuePerThread[startingQueue % internal_state.numThreads];
                    while (job_queue.pop_front(job))
                    {
                        JobDispatchArgs args;
                        args.groupID = job.groupID;
                        if (job.sharedmemory_size > 0)
                        {
                            thread_local static std::vector<uint8_t> shared_allocation_data;
                            shared_allocation_data.reserve(job.sharedmemory_size);
                            args.sharedmemory = shared_allocation_data.data();
                        }
                        else
                        {
                            args.sharedmemory = nullptr;
                        }

                        for (uint32_t i = job.groupJobOffset; i < job.groupJobEnd; ++i)
                        {
                            args.jobIndex = i;
                            args.groupIndex = i - job.groupJobOffset;
                            args.isFirstJobInGroup = (i == job.groupJobOffset);
                            args.isLastJobInGroup = (i == job.groupJobEnd - 1);
                            job.task(args);
                        }

                        job.ctx->counter.fetch_sub(1);
                    }
                    startingQueue++; // go to next queue
                }
            }

            void OnInit(uint32_t maxThreadCount)
            {
                SHAPE_PROFILE_FUNCTION();
                // Retrieve the number of hardware threads in this System:
                internal_state.numCores = std::thread::hardware_concurrency();

                if (internal_state.numThreads > 0)
                    return;
                maxThreadCount = std::max(1u, maxThreadCount);

                // Calculate the actual number of worker threads we want:
                internal_state.numThreads = Shape::Maths::Min(maxThreadCount, Shape::Maths::Max(1u, internal_state.numCores - 1));
                internal_state.jobQueuePerThread.reset(new JobQueue[internal_state.numThreads]);

                for (uint32_t threadID = 0; threadID < internal_state.numThreads; ++threadID)
                {
                    std::thread worker([threadID]
                        {
                            std::stringstream ss;
                    ss << "JobSystem_" << threadID;
                    SHAPE_PROFILE_SETTHREADNAME(ss.str().c_str());

                    std::shared_ptr<WorkerState> worker_state = internal_state.worker_state; // this is a copy of shared_ptr<WorkerState>, so it will remain alive for the thread's lifetime

                    while (worker_state->alive.load())
                    {
                        work(threadID);

                        // finished with jobs, put to sleep
                        std::unique_lock<std::mutex> lock(worker_state->wakeMutex);
                        worker_state->wakeCondition.wait(lock);
                    } });

                    // Do Windows-specific thread setup:
                    HANDLE handle = (HANDLE)worker.native_handle();

                    // Put each thread on to dedicated core
                    DWORD_PTR affinityMask = 1ull << threadID;
                    DWORD_PTR affinity_result = SetThreadAffinityMask(handle, affinityMask);
                    SHAPE_ASSERT(affinity_result > 0, "");

                    // Increase thread priority:
                    // BOOL priority_result = SetThreadPriority(handle, THREAD_PRIORITY_HIGHEST);
                    // LUMOS_ASSERT(priority_result != 0, "");

                    // Name the thread:
                    std::wstringstream wss;
                    wss << "JobSystem_" << threadID;
                    HRESULT hr = SetThreadDescription(handle, wss.str().c_str());

                    SHAPE_ASSERT(SUCCEEDED(hr), "");



                    worker.detach();
                }

                SHAPE_LOG_INFO("Initialised JobSystem with [{0} cores] [{1} threads]", internal_state.numCores, internal_state.numThreads);
            }

            uint32_t GetThreadCount()
            {
                return internal_state.numThreads;
            }

            void Execute(Context& ctx, const std::function<void(JobDispatchArgs)>& task)
            {
                // Context state is updated:
                ctx.counter.fetch_add(1);

                Job job;
                job.ctx = &ctx;
                job.task = task;
                job.groupID = 0;
                job.groupJobOffset = 0;
                job.groupJobEnd = 1;
                job.sharedmemory_size = 0;

                internal_state.jobQueuePerThread[internal_state.nextQueue.fetch_add(1) % internal_state.numThreads].push_back(job);
                internal_state.worker_state->wakeCondition.notify_one();
            }

            void Dispatch(Context& ctx, uint32_t jobCount, uint32_t groupSize, const std::function<void(JobDispatchArgs)>& task, size_t sharedmemory_size)
            {
                if (jobCount == 0 || groupSize == 0)
                {
                    return;
                }

                const uint32_t groupCount = DispatchGroupCount(jobCount, groupSize);

                // Context state is updated:
                ctx.counter.fetch_add(groupCount);

                Job job;
                job.ctx = &ctx;
                job.task = task;
                job.sharedmemory_size = (uint32_t)sharedmemory_size;

                for (uint32_t groupID = 0; groupID < groupCount; ++groupID)
                {
                    // For each group, generate one real job:
                    job.groupID = groupID;
                    job.groupJobOffset = groupID * groupSize;
                    job.groupJobEnd = std::min(job.groupJobOffset + groupSize, jobCount);

                    internal_state.jobQueuePerThread[internal_state.nextQueue.fetch_add(1) % internal_state.numThreads].push_back(job);
                }

                internal_state.worker_state->wakeCondition.notify_all();
            }

            uint32_t DispatchGroupCount(uint32_t jobCount, uint32_t groupSize)
            {
                // Calculate the amount of job groups to dispatch (overestimate, or "ceil"):
                return (jobCount + groupSize - 1) / groupSize;
            }

            bool IsBusy(const Context& ctx)
            {
                // Whenever the main thread label is not reached by the workers, it indicates that some worker is still alive
                return ctx.counter.load() > 0;
            }

            void Wait(const Context& ctx)
            {
                if (IsBusy(ctx))
                {
                    // Wake any threads that might be sleeping:
                    internal_state.worker_state->wakeCondition.notify_all();

                    // work() will pick up any jobs that are on stand by and execute them on this thread:
                    work(internal_state.nextQueue.fetch_add(1) % internal_state.numThreads);

                    while (IsBusy(ctx))
                    {
                        // If we are here, then there are still remaining jobs that work() couldn't pick up.
                        //    In this case those jobs are not standing by on a queue but currently executing
                        //    on other threads, so they cannot be picked up by this thread.
                        //    Allow to swap out this thread by OS to not spin endlessly for nothing
                        std::this_thread::yield();
                    }
                }
            }
        }
    }
}