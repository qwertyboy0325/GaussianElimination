#pragma once
#include <iostream>
#include <mutex>
#include <condition_variable>

class Barrier {
public:
    explicit Barrier(int);

    void wait();

private:
    int numThreads;
    int count;
    std::mutex mutex;
    std::condition_variable condition;
};