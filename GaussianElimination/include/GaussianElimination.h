#pragma once
#include <vector>


class Matrix {
public:
	Matrix(size_t rows, size_t cols);
	Matrix operator+(const Matrix& other) const;
	float& operator()(size_t row, size_t column);
	size_t getRows() const;
	size_t getCols() const;
private:
	size_t rows_;
	size_t cols_;
	std::vector<std::vector<float>> data_;
};

class MatrixMath {
public:
	static void GaussianElimination(Matrix&, int);
	static void GaussianElimination(Matrix&);
	float CalculateDeterminant(Matrix&);
private:

};
