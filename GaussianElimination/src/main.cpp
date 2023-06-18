#include <iostream>
#include <thread>
#include <random>
#include <chrono>
#include <string>
#include "../include/ThreadPool.h"
#include "../include/Matrix.h"
#include "../include/Barrier.h"
#include "./test/performance_test.h"
#include "command.h"

int main(int argc, char* argv[]) {
    CommandHandler commandHandler;

    if (argc < 2) {
        std::cout << "Invalid command. Please provide a valid command." << std::endl;
        return 0;
    }

    std::string input = argv[1];

    TaskCommand* command = commandHandler.getCommand(input);
    if (command == nullptr) {
        std::cout << "Invalid command. Please provide a valid command." << std::endl;
        return 0;
    }

    command->execute(argc, argv);

    return 0;
}
