#pragma once
struct PerformanceResult {
	size_t matrixSize = 0;
	std::vector<std::pair<size_t, double>> threadExecutionTimes;
};
std::vector<PerformanceResult> runPerformanceTest(int min_size, int max_size, int max_thread, int time_out, int numTests);
void printPerformanceTest(int min_size, int max_size, int max_thread, int time_out, int numTests);
std::tuple<double, bool> runSinglePerformanceTest(size_t matrixSize, size_t threads_num, int time_out, int numTests);

void exportToCSV(const std::vector<PerformanceResult>& results);
