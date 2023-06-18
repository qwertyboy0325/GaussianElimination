#include "command.h"

void TestCommand::execute(int argc, char* argv[]) {
	bool performance = false;
	bool random = false;
	bool print = false;
	bool file = false;
	std::string filename;
	std::string s_num, s_matrixScale;
	int num_count, matrix_scale;

	for (int i = 2; i < argc; ++i) {
		std::string suffix = argv[i];

		if (suffix == "-p" || suffix == "--performance") {
			performance = true;
			if (i + 1 < argc) {
				std::string nextSuffix = argv[i + 1];
				if (nextSuffix == "-t" || nextSuffix == "--print") {
					print = true;
					i++;
				}
				else if (nextSuffix == "-f" || nextSuffix == "--file") {
					file = true;
					if (i + 2 < argc) {
						filename = argv[i + 2];
						i += 2;
					}
				}
			}
		}
		else if (suffix == "-r" || suffix == "--random") {
			if (i + 1 < argc) {
				std::string s_num, s_matrixScale;
				num_count = std::stoi(s_num);
				matrix_scale = std::stoi(s_matrixScale);
			}
			random = true;
		}
	}

	// 執行相應的測試邏輯
	if (performance) {
		int min_size, max_size, max_thread, time_out, numTests;
		std::cout << "Enter the following parameters for the printPerformanceTest function:" << std::endl;
		std::cout << "Minimum size: ";
		std::cin >> min_size;

		std::cout << std::endl << "Maximum size: ";
		std::cin >> max_size;

		std::cout << std::endl << "Maximum number of threads: ";
		std::cin >> max_thread;

		std::cout << std::endl << "Timeout: ";
		std::cin >> time_out;

		std::cout << std::endl << "Number of tests: ";
		std::cin >> numTests;

		// 執行性能測試
		std::cout << "Running performance test..." << std::endl;
		if (print) {
			std::cout << "Printing performance results..." << std::endl;
			printPerformanceTest(min_size, max_size, max_thread, time_out, numTests);
		}
		if (file) {
			std::vector<PerformanceResult> result = runPerformanceTest(min_size, max_size, max_thread, time_out, numTests);
			std::cout << result.size();
			if (!filename.empty()) {
				exportToCSV(result, "");
			}
			else {
				exportToCSV(result, filename);
			}
		}
	}
	else if (random) {
		std::cout << "Running Random Matrix Generate to do GaussianElimination" << std::endl;
		while (true) {
			int row, col = 0;

			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<int> dis(num_count * 0.8f, num_count);

			row = dis(gen);
			col = dis(gen);
			Matrix matrix = MatrixUtility::GenerateRandomMatrix(row, col);

			std::cout << "Raw:\n" << matrix << std::endl;

			size_t threads_num = 0;
			if (matrix.getRows() < 20)
				threads_num = matrix.getRows();
			else
				threads_num = 20;
			ThreadPool thread_pool(threads_num, threads_num);
			Barrier barrier(threads_num);

			auto start_time = std::chrono::steady_clock::now();
			MatrixUtility::AvoidPivotZeroRow(matrix);
			for (int i = 0; i < threads_num; i++) {
				thread_pool.Enqueue([&matrix, &thread_pool, i, threads_num, &barrier] {MatrixMath::GaussianElimination(matrix, i, threads_num, barrier); });
			}
			thread_pool.WaitAll();
			auto end_time = std::chrono::steady_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
			std::cout << "Successed!" << std::endl;
			std::cout << "Result:\n" << matrix << std::endl;
		}
	}
	else {
		std::cout << "Invalid command. Please specify the -p or --performance suffix." << std::endl;
	}
}

void InputCommand::execute(int argc, char* argv[]) {
	std::cout << "Enter the following Matrix parameters for Gaussian Elimination:" << std::endl;
	Matrix matrix;
	std::cout << "Matrix row,col size:" << std::endl;
	std::cin >> matrix;
	size_t threads_num = 0;
	if (matrix.getRows() < 20)
		threads_num = matrix.getRows();
	else
		threads_num = 20;
	ThreadPool thread_pool(threads_num, threads_num);
	Barrier barrier(threads_num);

	MatrixUtility::AvoidPivotZeroRow(matrix);
	for (int i = 0; i < threads_num; i++) {
		thread_pool.Enqueue([&matrix, &thread_pool, i, threads_num, &barrier] {MatrixMath::GaussianElimination(matrix, i, threads_num, barrier); });
	}
	thread_pool.WaitAll();
	std::cout << "Result:\n" << matrix << std::endl;
}

void ReadCommand::execute(int argc, char* argv[]) {
	if (argc < 3) {
		std::cout << "Invalid command. Please provide at least one path." << std::endl;
		return;
	}

	std::cout << "Reading paths: ";
	std::queue<Matrix> matrixQueue;
	for (int i = 2; i < argc; ++i) {
		std::string path = argv[i];
		if (isValidPath(path)) {
			std::queue<Matrix> current_queue = MatrixUtility::ReadMatricesFromFile(path);
			while (!current_queue.empty()) {
				matrixQueue.push(current_queue.front());
				current_queue.pop();
			}
			while (!matrixQueue.empty()) {
				Matrix matrix = matrixQueue.front();
				size_t threads_num = 0;
				if (matrix.getRows() < 20)
					threads_num = matrix.getRows();
				else
					threads_num = 20;
				ThreadPool thread_pool(threads_num, threads_num);
				Barrier barrier(threads_num);

				MatrixUtility::AvoidPivotZeroRow(matrix);
				for (int i = 0; i < threads_num; i++) {
					thread_pool.Enqueue([&matrix, &thread_pool, i, threads_num, &barrier] {MatrixMath::GaussianElimination(matrix, i, threads_num, barrier); });
				}
				thread_pool.WaitAll();
				std::cout << "Result:\n" << matrix << std::endl;
				matrixQueue.pop();
			}
		}
		else {
			std::cout << "Invalid path: " << path << std::endl;
		}
	}
	std::cout << std::endl;
}

void HelpCommand::execute(int argc, char* argv[]) {
	std::cout << "Available commands:" << std::endl;
	std::cout << "1. test -p/--performance: Run performance test" << std::endl;
	std::cout << "		-p/--performance: Run performance test with the following options:" << std::endl;
	std::cout << "			-t/--print: Print performance results" << std::endl;
	std::cout << "			-f/--file <filename>: Export performance results to CSV file" << std::endl;
	std::cout << "		-r/--random <OutputQuantity> <MatrixScale>: Run random test" << std::endl;
	std::cout << "2. input: Execute input command" << std::endl;
	std::cout << "3. read <path1> [<path2> ...]: Read and process provided paths" << std::endl;
	std::cout << "4. help: Display this help message" << std::endl;
}

CommandHandler::CommandHandler() {
	commands["test"] = new TestCommand();
	commands["input"] = new InputCommand();
	commands["i"] = commands["input"];
	commands["read"] = new ReadCommand();
	commands["help"] = new HelpCommand();
}

CommandHandler::~CommandHandler() {
	for (auto& pair : commands) {
		delete pair.second;
	}
}

TaskCommand* CommandHandler::getCommand(const std::string& commandName) {
	if (commands.find(commandName) != commands.end()) {
		return commands[commandName];
	}
	return nullptr;
}
