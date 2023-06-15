#include "Barrier.h"

Barrier::Barrier(int numThreads) : numThreads(numThreads), count(0) {}

void Barrier::wait() {
	std::unique_lock<std::mutex> lock(mutex);

	count++;

	if (count < numThreads) {
		// 等待其他線程到達障礙
		condition.wait(lock, [this] { return count == numThreads; });
	}
	else {
		// 最後一個線程到達障礙，發出信號唤醒所有線程
		condition.notify_all();
	}
}