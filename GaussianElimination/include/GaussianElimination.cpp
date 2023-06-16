#include "GaussianElimination.h"
#include "ThreadPool.h"
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>


size_t Matrix::getRows() const {
	return rows_;
}
size_t Matrix::getCols() const {
	return cols_;
}
float& Matrix::operator()(size_t row, size_t column) {
	return data_[row][column];
}

inline Matrix::Matrix(size_t rows, size_t cols) :rows_(rows), cols_(cols), data_(rows, std::vector<float>(cols, 0.0f)) {}

Matrix Matrix::operator+(const Matrix& other) const {
	if (rows_ != other.rows_ || cols_ != other.cols_) {
		throw std::runtime_error("Matrix dimensions mismatch");
	}

	Matrix result(rows_, cols_);
	for (size_t i = 0; i < rows_; i++) {
		for (size_t j = 0; j < cols_; j++) {
			result(i, j) = data_[i][j] + other.data_[i][j];
		}
	}

	return result;
}

void MatrixMath::GaussianElimination(Matrix& matrix)
{
}

class SubMatrixResult {
	friend MatrixMath;
protected:
	float determinant;
	std::mutex mutex;
};


float MatrixMath::CalculateDeterminant(Matrix& matrix)
{
	if (matrix.getCols() != matrix.getRows()) {
		throw std::invalid_argument("From CaculateDeterminant(Matrix): Matrix must be square.");
		//return 0.0f;
	}
	size_t n = matrix.getCols();
	if (n == 1)
		return matrix(0, 0);
	else {
		float determinant = 0.0f;
		std::mutex determinantMutex;

		ThreadPool threadPool(n);
		std::vector<SubMatrixResult> subMatrixResults(n);

		for (size_t i = 0; i < n; i++) {
			Matrix subMatrix(n - 1, n - 1);
			for (size_t j = 1; j < n; j++) {
				size_t k = 0;
				for (size_t m = 0; m < n; m++) {
					if (m != i) {
						subMatrix(j - 1, k++) = matrix(j, m);
					}
				}
			}
			// 將子矩陣的計算任務提交到 ThreadPool 中
			threadPool.Enqueue([this, &subMatrix, i, &subMatrixResults]() {
				float subDeterminant = CalculateDeterminant(subMatrix);
				std::lock_guard<std::mutex> lock(subMatrixResults[i].mutex);
				subMatrixResults[i].determinant = subDeterminant;
				});
		}

		threadPool.WaitAll();

		for (size_t i = 0; i < n; i++) {
			std::lock_guard<std::mutex> lock(subMatrixResults[i].mutex);
			determinant += matrix(0, i) * subMatrixResults[i].determinant;
		}
		return determinant;
	}
}

void MatrixMath::GaussianElimination(Matrix&, int)
{
}
