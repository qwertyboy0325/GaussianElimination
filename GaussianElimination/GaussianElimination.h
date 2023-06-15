#pragma once
#include <vector>

class GaussianElimination {
public:
};

class Matrix {
public:
	Matrix(size_t rows, size_t cols) :rows_(rows), cols_(cols), data_(rows, std::vector<float>(cols, 0.0f)) {}
	Matrix operator+(const Matrix& other) const;
	float& operator()(size_t row, size_t column);
	size_t getRows() const;
	size_t getCols() const;
private:
	size_t rows_;
	size_t cols_;
	std::vector<std::vector<float>> data_;
};