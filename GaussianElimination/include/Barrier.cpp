#include "Barrier.h"

Barrier::Barrier(int numThreads) : numThreads(numThreads), count(0),phase(0) {}

void Barrier::wait() {
    std::unique_lock<std::mutex> lock(mutex);

    size_t currentPhase = phase;

    count++;

    if (count < numThreads) {
        condition.wait(lock, [this, currentPhase] { return phase != currentPhase; });
    }
    else {
        phase++;
        count = 0;
        condition.notify_all();
    }
}