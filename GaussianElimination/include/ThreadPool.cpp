#include "ThreadPool.h"
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>

inline ThreadPool::ThreadPool(size_t numThreads) : stop(false) {
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
				}
				task();
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

void ThreadPool::WaitAll()
{
	std::unique_lock<std::mutex> lock(queueMutex);
	condition.wait(lock, [this] {return tasks.empty(); });
}

template<class T>
void ThreadPool::Enqueue(T&& t)
{
	{
		std::unique_lock<std::mutex> lock(queueMutex);
		tasks.emplace(std::forward<T>(t));
	}
	condition.notify_one();
}

