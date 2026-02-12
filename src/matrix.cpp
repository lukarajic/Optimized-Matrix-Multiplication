#include "matrix.h"
#include <stdexcept>
#include <algorithm>
#include <thread>
#include <vector>

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
                // We use vld1q_f32 for NEON. While it handles unaligned,
                // our data is now 64-byte aligned at the start of rows.
                float32x4_t vB = vld1q_f32(&B(k, j));
                float32x4_t vC = vld1q_f32(&C(i, j));
                vC = vfmaq_f32(vC, vA, vB); 
                vst1q_f32(&C(i, j), vC);
            }
#elif defined(__AVX2__)
            __m256 vA = _mm256_set1_ps(rA);
            for (; j + 7 < B.cols; j += 8) {
                // Use aligned loads/stores for AVX2 (requires 32-byte alignment)
                // Note: Only valid if cols is multiple of 8, or if we handle boundaries
                __m256 vB = _mm256_load_ps(&B(k, j));
                __m256 vC = _mm256_load_ps(&C(i, j));
                vC = _mm256_fmadd_ps(vA, vB, vC);
                _mm256_store_ps(&C(i, j), vC);
            }
#endif

            // Cleanup loop for remaining elements
            for (; j < B.cols; ++j) {
                C(i, j) += rA * B(k, j);
            }
        }
    }
}

void multiply_optimized_v5_threaded(const Matrix& A, const Matrix& B, Matrix& C, unsigned int numThreads) {
    if (A.cols != B.rows || C.rows != A.rows || C.cols != B.cols) {
        throw std::invalid_argument("Matrix dimensions mismatch.");
    }

    if (numThreads == 0) {
        numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0) numThreads = 2; // Fallback
    }

    std::fill(C.data.begin(), C.data.end(), 0.0f);

    auto worker = [&](size_t startRow, size_t endRow) {
        for (size_t i = startRow; i < endRow; ++i) {
            for (size_t k = 0; k < A.cols; ++k) {
                float rA = A(i, k);
                for (size_t j = 0; j < B.cols; ++j) {
                    C(i, j) += rA * B(k, j);
                }
            }
        }
    };

    std::vector<std::thread> threads;
    size_t rowsPerThread = A.rows / numThreads;

    for (unsigned int t = 0; t < numThreads; ++t) {
        size_t startRow = t * rowsPerThread;
        size_t endRow = (t == numThreads - 1) ? A.rows : (t + 1) * rowsPerThread;
        if (startRow < endRow) {
            threads.emplace_back(worker, startRow, endRow);
        }
    }

    for (auto& t : threads) {
        t.join();
    }
}

void multiply_optimized_v6_register_blocked_2x2(const Matrix& A, const Matrix& B, Matrix& C) {
    if (A.cols != B.rows || C.rows != A.rows || C.cols != B.cols) {
        throw std::invalid_argument("Matrix dimensions mismatch.");
    }

    std::fill(C.data.begin(), C.data.end(), 0.0f);

    size_t n = A.rows; // Assuming square for simplicity of logic, but works for rect
    
    for (size_t i = 0; i < n; i += 2) {
        // Handle odd row at bottom
        if (i + 1 >= n) {
            for (size_t k = 0; k < A.cols; ++k) {
                float rA = A(i, k);
                for (size_t j = 0; j < B.cols; ++j) {
                    C(i, j) += rA * B(k, j);
                }
            }
            continue;
        }

        for (size_t k = 0; k < A.cols; ++k) {
            float rA0 = A(i, k);
            float rA1 = A(i + 1, k);

            size_t j = 0;
            // Main 2x2 loop
            for (; j + 1 < B.cols; j += 2) {
                float rB0 = B(k, j);
                float rB1 = B(k, j + 1);

                C(i, j)     += rA0 * rB0;
                C(i, j + 1) += rA0 * rB1;
                C(i + 1, j) += rA1 * rB0;
                C(i + 1, j + 1) += rA1 * rB1;
            }
            
            // Cleanup columns
            for (; j < B.cols; ++j) {
                float rB = B(k, j);
                C(i, j) += rA0 * rB;
                C(i + 1, j) += rA1 * rB;
            }
        }
    }
}

void multiply_optimized_v7_threaded_register_blocked(const Matrix& A, const Matrix& B, Matrix& C, unsigned int numThreads) {
    if (A.cols != B.rows || C.rows != A.rows || C.cols != B.cols) {
        throw std::invalid_argument("Matrix dimensions mismatch.");
    }

    if (numThreads == 0) {
        numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0) numThreads = 2; // Fallback
    }

    std::fill(C.data.begin(), C.data.end(), 0.0f);

    size_t n = A.rows; // Assuming square matrices for simplicity

    auto worker = [&](size_t startRow, size_t endRow) {
        for (size_t i = startRow; i < endRow; i += 2) {
            // Handle odd row at bottom
            if (i + 1 >= n) {
                for (size_t k = 0; k < A.cols; ++k) {
                    float rA = A(i, k);
                    for (size_t j = 0; j < B.cols; ++j) {
                        C(i, j) += rA * B(k, j);
                    }
                }
                continue;
            }

            for (size_t k = 0; k < A.cols; ++k) {
                float rA0 = A(i, k);
                float rA1 = A(i + 1, k);

                size_t j = 0;
                // Main 2x2 loop
                for (; j + 1 < B.cols; j += 2) {
                    float rB0 = B(k, j);
                    float rB1 = B(k, j + 1);

                    C(i, j)     += rA0 * rB0;
                    C(i, j + 1) += rA0 * rB1;
                    C(i + 1, j) += rA1 * rB0;
                    C(i + 1, j + 1) += rA1 * rB1;
                }
                
                // Cleanup columns
                for (; j < B.cols; ++j) {
                    float rB = B(k, j);
                    C(i, j) += rA0 * rB;
                    C(i + 1, j) += rA1 * rB;
                }
            }
        }
    };

    std::vector<std::thread> threads;
    size_t rowsPerThread = (n + numThreads - 1) / numThreads; // ceiling division

    for (unsigned int t = 0; t < numThreads; ++t) {
        size_t startRow = t * rowsPerThread;
        size_t endRow = std::min(startRow + rowsPerThread, n);
        if (startRow < endRow) {
            threads.emplace_back(worker, startRow, endRow);
        }
    }

    for (auto& t : threads) {
        t.join();
    }
}

void multiply_optimized_v8_prefetch(const Matrix& A, const Matrix& B, Matrix& C) {
    if (A.cols != B.rows || C.rows != A.rows || C.cols != B.cols) {
        throw std::invalid_argument("Matrix dimensions mismatch.");
    }

    std::fill(C.data.begin(), C.data.end(), 0.0f);

    for (size_t i = 0; i < A.rows; ++i) {
        for (size_t k = 0; k < A.cols; ++k) {
            float rA = A(i, k);
            const float* b_row = &B(k, 0);
            float* c_row = &C(i, 0);
            
            for (size_t j = 0; j < B.cols; ++j) {
                // Prefetch B and C data 16 elements ahead (64 bytes / 1 typical cache line)
                __builtin_prefetch(&b_row[j + 16], 0, 3); // 0=read, 3=high temporal locality
                __builtin_prefetch(&c_row[j + 16], 1, 3); // 1=write
                
                c_row[j] += rA * b_row[j];
            }
        }
    }
}
