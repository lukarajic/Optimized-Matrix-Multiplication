#include "matrix.h"
#include <stdexcept>

void multiply_naive(const Matrix& A, const Matrix& B, Matrix& C) {
    if (A.cols != B.rows) {
        throw std::invalid_argument("Matrix dimensions mismatch for multiplication.");
    }
    if (C.rows != A.rows || C.cols != B.cols) {
        throw std::invalid_argument("Output matrix dimensions mismatch.");
    }

    for (size_t i = 0; i < A.rows; ++i) {
        for (size_t j = 0; j < B.cols; ++j) {
            float sum = 0.0f;
            for (size_t k = 0; k < A.cols; ++k) {
                sum += A(i, k) * B(k, j);
            }
            C(i, j) = sum;
        }
    }
}

void multiply_optimized_v1(const Matrix& A, const Matrix& B, Matrix& C) {
    if (A.cols != B.rows || C.rows != A.rows || C.cols != B.cols) {
        throw std::invalid_argument("Matrix dimensions mismatch.");
    }

    // Initialize C to zero (unlike naive, we accumulate directly into C)
    std::fill(C.data.begin(), C.data.end(), 0.0f);

    for (size_t i = 0; i < A.rows; ++i) {
        for (size_t k = 0; k < A.cols; ++k) {
            float rA = A(i, k);
            for (size_t j = 0; j < B.cols; ++j) {
                C(i, j) += rA * B(k, j);
            }
        }
    }
}
