#include "Matrix.h"
#include "ThreadPool.h"
#include "Barrier.h"
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <random>


std::mutex cout_mutex;
size_t Matrix::getRows() const {
	return rows_;
}
size_t Matrix::getCols() const {
	return cols_;
}

size_t Matrix::getElementWidth(size_t row, size_t column) const {
	return std::to_string(data_[row][column]).length();
}

float& Matrix::operator()(size_t row, size_t column) {
	return data_[row][column];
}

Matrix::Matrix(size_t rows, size_t cols) :rows_(rows), cols_(cols), data_(rows, std::vector<float>(cols, 0.0f)) {}

Matrix::Matrix() : rows_(0), cols_(0), data_() {}

Matrix::~Matrix()
{
}



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

Matrix& Matrix::operator=(const Matrix& other) {
	// 檢查是否是自我賦值
	if (this == &other) {
		return *this;
	}

	// 深度複製 other 的內容到目前的物件
	rows_ = other.rows_;
	cols_ = other.cols_;
	data_ = std::vector<std::vector<float>>(rows_, std::vector<float>(cols_, 0.0f));
	for (size_t i = 0; i < rows_; i++) {
		for (size_t j = 0; j < cols_; j++) {
			data_[i][j] = other.data_[i][j];
		}
	}

	return *this;
}



class SubMatrixResult {
	friend MatrixMath;
public:
	SubMatrixResult() {
		determinant = 0.0f;
	}
protected:

	float determinant = 0.0f;
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

		ThreadPool threadPool(n, n);
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


void MatrixMath::GaussianElimination(Matrix& matrix, size_t thread_id,size_t max_thread_id, Barrier& barrier)
{
	const size_t rows = matrix.getRows();
	const size_t cols = matrix.getCols();

	for (size_t i = 0; i < rows; i++) {
		// 將對角線元素調整為 1
		if (i < cols) {
			if (thread_id == i % max_thread_id) {
				double factor = matrix(i, i);
				if (factor != 0) {
					for (size_t j = i; j < cols; j++)
						matrix(i, j) /= factor;
				}
			}
		}

		// 等待所有線程完成當前階段的操作
		barrier.wait();

		// 進行消去操作
		for (size_t j = 0; j < rows; j++) {
			if (thread_id == j % max_thread_id && j != i && i < cols) {
				double factor = matrix(j, i);
				for (size_t k = i; k < cols; k++)
				{
					matrix(j, k) -= factor * matrix(i, k);
				}
			}
		}

		// 等待所有線程完成當前階段的操作
		barrier.wait();
	}
	return;
}






void MatrixMath::GaussianElimination(Matrix&)
{
}

std::istream& operator>>(std::istream& is, Matrix& matrix) {
	size_t rows, cols;
	is >> rows >> cols;

	matrix = Matrix(rows, cols);

	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < cols; j++) {
			is >> matrix.data_[i][j];
		}
	}

	return is;
}

std::ostream& operator<<(std::ostream& os, const Matrix& matrix) {
	for (size_t i = 0; i < matrix.getRows(); i++) {
		for (size_t j = 0; j < matrix.getCols(); j++) {
			os << matrix.data_[i][j] << ' ';
		}
		os << '\n';
	}

	return os;
}


Matrix MatrixUtility::GenerateRandomMatrix(size_t rows, size_t cols) {
	Matrix matrix(rows, cols);
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(-50, 50);

	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < cols; j++) {
			matrix(i, j) = dis(gen);
		}
	}

	return matrix;
}

void MatrixUtility::AvoidPivotZeroRow(Matrix& matrix)
{
	size_t n = matrix.getRows();
	size_t cols = matrix.getCols();
	size_t minSize = std::min(n, cols);

	for (size_t i = 0; i < minSize; i++) {
		if (matrix(i, i) == 0) {
			// 找尋非零行進行交換
			size_t swapRowIndex = i;
			for (size_t j = i + 1; j < n; j++) {
				if (matrix(j, i) != 0) {
					swapRowIndex = j;
					break;
				}
			}

			// 交換行
			if (swapRowIndex != i) {
				for (size_t j = 0; j < cols; j++) {
					std::swap(matrix(i, j), matrix(swapRowIndex, j));
				}
			}
		}
	}
}

