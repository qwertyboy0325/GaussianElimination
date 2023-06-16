#pragma once
#include "Barrier.h"
#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadPool {
public:
	ThreadPool(size_t numThreads,size_t);
	~ThreadPool();
	template<class T>
	void Enqueue(T&&);
	void WaitAll();
private:
	std::vector<std::thread> threads;
	std::queue<std::function<void()>> tasks;
	std::mutex queueMutex;
	std::condition_variable condition;
	int taskCount;
	size_t submittedTasks;
	size_t totalTasks;
	std::condition_variable taskFinishedCondition;
	bool stop;
	bool started;
};

template<class T>
void ThreadPool::Enqueue(T&& t)
{
	{
		std::unique_lock<std::mutex> lock(queueMutex);
		tasks.emplace(std::forward<T>(t));
	}
	condition.notify_one();
}