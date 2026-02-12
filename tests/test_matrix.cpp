#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include "matrix.h"

bool are_matrices_equal(const Matrix& A, const Matrix& B, float epsilon = 1e-4f) {
    if (A.rows != B.rows || A.cols != B.cols) return false;
    for (size_t i = 0; i < A.data.size(); ++i) {
        if (std::abs(A.data[i] - B.data[i]) > epsilon) {
            std::cout << "Mismatch at index " << i << ": " << A.data[i] << " vs " << B.data[i] << std::endl;
            return false;
        }
    }
    return true;
}

void fill_random(Matrix& M) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
    for (auto& val : M.data) val = dis(gen);
}

int main() {
    const size_t size = 128;
    Matrix A(size, size), B(size, size), Expected(size, size);
    fill_random(A);
    fill_random(B);

    std::cout << "Running correctness tests (Size " << size << "x" << size << ")..." << std::endl;

    // Baseline
    multiply_naive(A, B, Expected);

    struct TestCase {
        std::string name;
        void (*func)(const Matrix&, const Matrix&, Matrix&);
    };

    std::vector<TestCase> cases = {
        {"Opt V1 (i-k-j)", multiply_optimized_v1},
        {"Opt V3 (Unroll)", multiply_optimized_v3_unrolled},
        {"Opt V4 (SIMD)", multiply_optimized_v4_simd},
        {"Opt V5 (Thread)", [](const Matrix& A, const Matrix& B, Matrix& C) { multiply_optimized_v5_threaded(A, B, C); }},
        {"Opt V6 (RegBlk)", multiply_optimized_v6_register_blocked_2x2},
        {"Opt V7 (Thread+RegBlk)", [](const Matrix& A, const Matrix& B, Matrix& C) { multiply_optimized_v7_threaded_register_blocked(A, B, C); }},
        {"Opt V8 (Prefetch)", multiply_optimized_v8_prefetch}
    };

    bool all_passed = true;
    for (const auto& c : cases) {
        Matrix Result(size, size);
        try {
            c.func(A, B, Result);
            if (are_matrices_equal(Expected, Result)) {
                std::cout << "[PASS] " << c.name << std::endl;
            } else {
                std::cout << "[FAIL] " << c.name << " (Result mismatch)" << std::endl;
                all_passed = false;
            }
        } catch (const std::exception& e) {
            std::cout << "[ERROR] " << c.name << ": " << e.what() << std::endl;
            all_passed = false;
        }
    }

    // Special case for V2 (Tiled) because it takes an extra argument
    {
        Matrix Result(size, size);
        multiply_optimized_v2_tiled(A, B, Result, 32);
        if (are_matrices_equal(Expected, Result)) {
            std::cout << "[PASS] Opt V2 (Tiled)" << std::endl;
        } else {
            std::cout << "[FAIL] Opt V2 (Tiled)" << std::endl;
            all_passed = false;
        }
    }

    return all_passed ? 0 : 1;
}
