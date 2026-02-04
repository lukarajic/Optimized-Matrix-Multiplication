#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <cstddef>

struct Matrix {
    size_t rows;
    size_t cols;
    std::vector<float> data;

    Matrix(size_t r, size_t c) : rows(r), cols(c), data(r * c, 0.0f) {}

    float& operator()(size_t r, size_t c) {
        return data[r * cols + c];
    }

    const float& operator()(size_t r, size_t c) const {
        return data[r * cols + c];
    }
};

// Naive triple-loop matrix multiplication: C = A * B
void multiply_naive(const Matrix& A, const Matrix& B, Matrix& C);

// Optimization v1: Loop Reordering (i-k-j) for better cache locality
void multiply_optimized_v1(const Matrix& A, const Matrix& B, Matrix& C);

// Optimization v2: Tiling (Blocking) to improve temporal cache locality
void multiply_optimized_v2_tiled(const Matrix& A, const Matrix& B, Matrix& C, size_t blockSize = 32);

#endif // MATRIX_H
