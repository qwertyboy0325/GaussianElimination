#include "ThreadPool.h"
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>

ThreadPool::ThreadPool(size_t numThreads,size_t totalTasks) : stop(false), taskCount(0), started(false), submittedTasks(0),totalTasks(totalTasks) {

	for (size_t i = 0; i < numThreads; i++) {
		threads.emplace_back([this] {
			while (true) {
				std::function<void()> task;
				{
					started = true;
					std::unique_lock<std::mutex> lock(queueMutex);
					condition.wait(lock, [this] { return stop || !tasks.empty(); });
					if (stop && tasks.empty())
						return;
					task = std::move(tasks.front());
					tasks.pop();
					submittedTasks++;
					taskCount++;
				}
				task();
				taskCount--;
				taskFinishedCondition.notify_all();
			}
			});
	}
}

ThreadPool::~ThreadPool() {
	{
		std::unique_lock<std::mutex> lock(queueMutex);
		stop = true;
		started = false;
	}
	condition.notify_all();
	for (std::thread& thread : threads)
		thread.join();
}

void ThreadPool::WaitAll() {
	std::unique_lock<std::mutex> lock(queueMutex);
	taskFinishedCondition.wait(lock, [this] { return taskCount == 0 && submittedTasks == totalTasks; });
}