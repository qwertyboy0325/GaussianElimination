#include "performance_test.h"

std::vector<PerformanceResult> runPerformanceTest(int min_size, int max_size, int max_thread, int time_out, int numTests) {
    std::vector<PerformanceResult> results;

    if (min_size < 2 || min_size > max_size)
        return results;

    for (int i = min_size; i <= max_size; i++) {
        size_t matrixSize = static_cast<size_t>(i);
        PerformanceResult result;
        result.matrixSize = matrixSize;

        for (size_t threads_num = 1; threads_num <= max_thread; threads_num++) {
            double totalExecutionTime = 0.0;
            int numValidTests = 0;

            for (int test = 0; test < numTests; test++) {
                double executionTime;
                bool testSkipped = false;
                std::tie(executionTime, testSkipped) = runSinglePerformanceTest(matrixSize, threads_num, time_out, numTests);

                totalExecutionTime += executionTime;
                if (!testSkipped) {
                    numValidTests++;
                }
            }

            if (numValidTests > 0) {
                double averageExecutionTime = totalExecutionTime / numValidTests;
                result.thread_num = threads_num;
                result.averageExecutionTime = averageExecutionTime;
                results.push_back(result);
            }
        }
    }
    return results;
}

void printPerformanceTest(int min_size, int max_size, int max_thread, int time_out, int numTests) {
    if (min_size < 2 || min_size > max_size)
        return;

    for (int i = min_size; i <= max_size; i++) {
        size_t matrixSize = static_cast<size_t>(i);
        for (size_t threads_num = 1; threads_num <= max_thread; threads_num++) {
            double averageExecutionTime;
            bool testSkipped = false;
            std::tie(averageExecutionTime, testSkipped) = runSinglePerformanceTest(matrixSize, threads_num, time_out, numTests);

            if (!testSkipped) {
                std::cout << std::fixed << std::setprecision(4);
                std::cout << "Matrix Size: " << matrixSize << ", Threads num: " << threads_num
                    << ", Average Execution Time: " << averageExecutionTime << " milliseconds" << std::endl;
            }
        }
    }
}

std::tuple<double, bool> runSinglePerformanceTest(size_t matrixSize, size_t threads_num, int time_out, int numTests) {
    double totalExecutionTime = 0.0;
    int numSkippedTests = 0;

    for (int test = 0; test < numTests; test++) {
        int row, col = 0;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(matrixSize * 0.8f, matrixSize);

        row = dis(gen);
        col = dis(gen);

        Matrix matrix = MatrixUtility::GenerateRandomMatrix(row, col);

        ThreadPool thread_pool(threads_num, threads_num);
        Barrier barrier(threads_num);

        auto start_time = std::chrono::steady_clock::now();
        MatrixUtility::AvoidPivotZeroRow(matrix);
        for (size_t i = 0; i < threads_num; i++) {
            thread_pool.Enqueue([&matrix, &thread_pool, i, threads_num, &barrier] {
                MatrixMath::GaussianElimination(matrix, i, threads_num, barrier);
                });
        }
        thread_pool.WaitAll();
        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        double executionTime = duration.count();

        totalExecutionTime += executionTime;
        if (executionTime > time_out) {
            numSkippedTests++;
        }
    }

    int numValidTests = numTests - numSkippedTests;
    double averageExecutionTime = (numValidTests > 0) ? (totalExecutionTime / numValidTests) : 0.0;
    bool testSkipped = (numValidTests == 0);

    return std::make_tuple(averageExecutionTime, testSkipped);
}

void exportToCSV(const std::vector<PerformanceResult>& results, std::string custom_filename) {
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::filesystem::path outputPath = currentPath / "test";
    std::filesystem::create_directory(outputPath);

    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    struct tm timeinfo;
    localtime_s(&timeinfo, &time);
    ss << std::put_time(&timeinfo, "%Y%m%d_%H%M%S");

    std::filesystem::path filename;
    if (custom_filename.empty())
        filename = outputPath / ("performance_results_" + ss.str() + ".csv");
    else
        filename = outputPath / (custom_filename + ".csv");

    std::ofstream file(filename.string());
    if (!file.is_open()) {
        std::cout << "Failed to open file: " << filename << std::endl;
        return;
    }

    file << "Matrix Size, Thread Num, Average Execution Time" << std::endl;

    for (const auto& result : results) {
        file << result.matrixSize << ", ";
        file << result.thread_num << ", ";
        file << result.averageExecutionTime;
        file << std::endl;
    }

    file.close();
    std::cout << "CSV file has been exported: " << filename << std::endl;
}