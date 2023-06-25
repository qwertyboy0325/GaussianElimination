#pragma once
#define EPSILON_DOUBLE 1.0e-14
#include "ThreadPool.h"
#include "Barrier.h"
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>
#include <string>
#include <iomanip>
#include <cmath>
#include <sstream>
#include <random>
#include <queue>
#include <fstream>
#include <mutex>

extern std::mutex cout_mutex;

class Matrix {

public:
	Matrix(size_t rows, size_t cols);
	Matrix();
	~Matrix();
	Matrix operator+(const Matrix& other) const;
	Matrix& operator=(const Matrix& other);
	double& operator()(size_t row, size_t column);
	Matrix operator*(const Matrix& other) const;
	bool isSquare();
	size_t getRows() const;
	size_t getCols() const;
	size_t getElementWidth(size_t, size_t) const;
	friend std::istream& operator>>(std::istream& is, Matrix& matrix);
	friend std::ostream& operator<<(std::ostream& os, const Matrix& matrix);
	friend std::istringstream& operator>>(std::istringstream& iss, Matrix& matrix);
private:
	size_t rows_;
	size_t cols_;
	std::vector<std::vector<double>> data_;
};

std::istream& operator>>(std::istream& is, Matrix& matrix);
std::ostream& operator<<(std::ostream& os, const Matrix& matrix);
std::istringstream& operator>>(std::istringstream& iss, Matrix& matrix);

class MatrixMath {
public:
	void GaussianElimination(Matrix&);
	double CalculateDeterminant(Matrix& ref); //cofactor expansion
	double CaculateDeterminant(Matrix& L, Matrix& U);
	static void MatrixPivotting(Matrix& ref, size_t started_row, size_t current_thread_id, size_t max_thread_id, Barrier& barrier);
	static void GaussianElimination(Matrix& ref, size_t current_thread_id, size_t max_thread_id, Barrier& barrier);
	static void GaussianEliminationLU(Matrix& A, Matrix& L, Matrix& U, size_t current_thread_id, size_t max_thread_id, Barrier& barrier);
	Matrix* Transpose(Matrix& ref, size_t current_thread_id, size_t max_thread_id, Barrier& barrier);
	//static Matrix* Transpose(Matrix& ref, size_t current_thread_id, size_t max_thread_id, Barrier& barrier);
private:


};

class MatrixUtility {
public:
	static Matrix GenerateRandomMatrix(size_t rows, size_t cols);
	static void AvoidPivotZeroRow(Matrix&);
	static std::queue<Matrix> ReadMatricesFromFile(const std::string& filename);
	static Matrix GetIdentityMatrix(size_t scale);
};
