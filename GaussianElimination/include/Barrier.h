#pragma once
#include <iostream>
#include <mutex>
#include <condition_variable>

class Barrier {
public:
    explicit Barrier(size_t);

    void wait();

private:
    int numThreads;
    int count;
    size_t phase;
    std::mutex mutex;
    std::condition_variable condition;
};