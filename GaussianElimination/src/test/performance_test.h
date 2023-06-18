#pragma once
#include <iostream>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <random>
#include <chrono>
#include <ctime>
#include <vector>
#include <sstream>
#include "../../include/ThreadPool.h"
#include "../../include/Matrix.h"
#include "../../include/Barrier.h"

struct PerformanceResult {
    size_t matrixSize = 0;
    size_t thread_num = 0;
    double averageExecutionTime = 0.0;
};
std::vector<PerformanceResult> runPerformanceTest(int min_size, int max_size, int max_thread, int time_out, int numTests);
void printPerformanceTest(int min_size, int max_size, int max_thread, int time_out, int numTests);
std::tuple<double, bool> runSinglePerformanceTest(size_t matrixSize, size_t threads_num, int time_out, int numTests);

void exportToCSV(const std::vector<PerformanceResult>& results,std::string custom_filename);
