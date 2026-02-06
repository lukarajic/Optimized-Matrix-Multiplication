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

void multiply_optimized_v2_tiled(const Matrix& A, const Matrix& B, Matrix& C, size_t blockSize) {
    if (A.cols != B.rows || C.rows != A.rows || C.cols != B.cols) {
        throw std::invalid_argument("Matrix dimensions mismatch.");
    }

    std::fill(C.data.begin(), C.data.end(), 0.0f);

    size_t n = A.rows; // Assuming square matrices for simplicity in loop bounds, but logic holds

    // Loop over blocks
    for (size_t ii = 0; ii < n; ii += blockSize) {
        for (size_t kk = 0; kk < n; kk += blockSize) {
            for (size_t jj = 0; jj < n; jj += blockSize) {

                // Loop inside the blocks (handling boundary conditions)
                size_t i_max = std::min(ii + blockSize, n);
                size_t k_max = std::min(kk + blockSize, n);
                size_t j_max = std::min(jj + blockSize, n);

                for (size_t i = ii; i < i_max; ++i) {
                    for (size_t k = kk; k < k_max; ++k) {
                        float rA = A(i, k);
                        for (size_t j = jj; j < j_max; ++j) {
                            C(i, j) += rA * B(k, j);
                        }
                    }
                }
            }
        }
    }
}

void multiply_optimized_v3_unrolled(const Matrix& A, const Matrix& B, Matrix& C) {
    if (A.cols != B.rows || C.rows != A.rows || C.cols != B.cols) {
        throw std::invalid_argument("Matrix dimensions mismatch.");
    }

    std::fill(C.data.begin(), C.data.end(), 0.0f);

    for (size_t i = 0; i < A.rows; ++i) {
        for (size_t k = 0; k < A.cols; ++k) {
            float rA = A(i, k);
            size_t j = 0;
            // Unroll by 4
            for (; j + 3 < B.cols; j += 4) {
                C(i, j) += rA * B(k, j);
                C(i, j + 1) += rA * B(k, j + 1);
                C(i, j + 2) += rA * B(k, j + 2);
                C(i, j + 3) += rA * B(k, j + 3);
            }
            // Cleanup loop
            for (; j < B.cols; ++j) {
                C(i, j) += rA * B(k, j);
            }
        }
    }
}

void multiply_optimized_v4_simd(const Matrix& A, const Matrix& B, Matrix& C) {
    if (A.cols != B.rows || C.rows != A.rows || C.cols != B.cols) {
        throw std::invalid_argument("Matrix dimensions mismatch.");
    }

    std::fill(C.data.begin(), C.data.end(), 0.0f);

    for (size_t i = 0; i < A.rows; ++i) {
        for (size_t k = 0; k < A.cols; ++k) {
            float rA = A(i, k);
            size_t j = 0;

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
            float32x4_t vA = vdupq_n_f32(rA);
            for (; j + 3 < B.cols; j += 4) {
                float32x4_t vB = vld1q_f32(&B(k, j));
                float32x4_t vC = vld1q_f32(&C(i, j));
                // vC = vA * vB + vC
                vC = vfmaq_f32(vC, vA, vB); 
                vst1q_f32(&C(i, j), vC);
            }
#elif defined(__AVX2__)
            __m256 vA = _mm256_set1_ps(rA);
            for (; j + 7 < B.cols; j += 8) {
                __m256 vB = _mm256_loadu_ps(&B(k, j));
                __m256 vC = _mm256_loadu_ps(&C(i, j));
                vC = _mm256_fmadd_ps(vA, vB, vC);
                _mm256_storeu_ps(&C(i, j), vC);
            }
#endif

            // Cleanup loop for remaining elements
            for (; j < B.cols; ++j) {
                C(i, j) += rA * B(k, j);
            }
        }
    }
}
