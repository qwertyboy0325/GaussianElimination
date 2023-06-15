#include "GaussianElimination.h"
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

Matrix Matrix::operator+(const Matrix& other) const {
    if (rows_ != other.rows_ || cols_ != other.cols_) {
        throw std::runtime_error("Matrix dimensions mismatch");
    }

    Matrix result(rows_, cols_);
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            result(i, j) = data_[i][j] + other.data_[i][j];
        }
    }

    return result;
}