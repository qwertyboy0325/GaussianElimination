#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t numThreads, size_t totalTasks) : stop(false), taskCount(0),
submittedTasks(0), totalTasks(totalTasks), completedTasks(0) {

	for (size_t i = 0; i < numThreads; i++) {
		threads.emplace_back([this] {
			while (true) {
				std::function<void()> task;
				{
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
				{
					std::lock_guard<std::mutex> lock(completedTasksMutex);
					completedTasks++;
				}
				taskFinishedCondition.notify_all();
			}
			});
	}
}

ThreadPool::~ThreadPool() {
	{
		std::unique_lock<std::mutex> lock(queueMutex);
		stop = true;
	}
	condition.notify_all();
	for (std::thread& thread : threads)
		thread.join();
}

void ThreadPool::WaitAll() {
	std::unique_lock<std::mutex> lock(queueMutex);
	taskFinishedCondition.wait(lock, [this] { return submittedTasks == totalTasks; });
}