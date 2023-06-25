#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <string>
#include <map>
#include "../include/ThreadPool.h"
#include "../include/Matrix.h"
#include "../include/Barrier.h"
#include "test/performance_test.h"
#include "Utility/FilepathTool.h"

class TaskCommand {
public:
	virtual ~TaskCommand() {}
	virtual void execute(int argc, char* argv[]) = 0;
};

class TestCommand : public TaskCommand {
public:
	void execute(int argc, char* argv[]) override;
};

class InputCommand : public TaskCommand {
public:
	void execute(int argc, char* argv[]) override;
};

class ReadCommand : public TaskCommand {
public:
	void execute(int argc, char* argv[]) override;
};

class HelpCommand : public TaskCommand {
public:
	void execute(int argc, char* argv[]) override;
};

//class CalculateCommand : public TaskCommand {
//public:
//	std::map<std::string, Matrix> matrices;
//	std::map<std::string, std::string> action;
//	void execute(int argc, char* argv[]) override;
//	void printGuide();
//};

class CommandHandler {
private:
	std::unordered_map<std::string, TaskCommand*> commands;

public:
	CommandHandler();
	~CommandHandler();

	TaskCommand* getCommand(const std::string& commandName);
};