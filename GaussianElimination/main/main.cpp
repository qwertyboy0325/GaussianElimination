﻿#include <iostream>
#include <thread>
#include <random>
#include <chrono>
#include "../include/ThreadPool.h"
#include "../include/Matrix.h"
#include "../include/Barrier.h"

int main()
{
	int num = 0;
	std::cin >> num;
	for (int i = 0; i < num; i++) {
		int row, col = 0;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> dis(5, 15);

		row = dis(gen);
		col = dis(gen);
		Matrix matrix = MatrixUtility::GenerateRandomMatrix(row, col);
		std::cout << "Raw:\n" << matrix << std::endl;
		size_t threads_num = matrix.getRows();
		ThreadPool thread_pool(threads_num, threads_num);
		Barrier barrier(threads_num);

		auto start_time = std::chrono::steady_clock::now();
		MatrixUtility::AvoidPivotZeroRow(matrix);
		for (int i = 0; i < threads_num; i++) {
			thread_pool.Enqueue([&matrix, i, &barrier] {MatrixMath::GaussianElimination(matrix, i, barrier); });
		}
		thread_pool.WaitAll();
		auto end_time = std::chrono::steady_clock::now();
		//std::cout << "Result:" << std::endl << matrix << std::endl<< std::endl;
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
		std::cout << "Successed!" << std::endl;
		std::cout <<"Result:\n" << matrix << std::endl;
		std::cout << "ThreadPool execution time: " << duration.count() << " milliseconds" << std::endl;
	}
}
