#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include "matrix.h"

// Helper function to measure performance
void run_benchmark(size_t size, int iterations) {
    Matrix A(size, size);
    Matrix B(size, size);
    Matrix C(size, size);

    // Initialize matrices with some values
    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < size; ++j) {
            A(i, j) = static_cast<float>(i + j);
            B(i, j) = static_cast<float>(i - j);
        }
    }

    // Warmup run
    multiply_naive(A, B, C);

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        multiply_naive(A, B, C);
    }
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    double avg_time = elapsed.count() / iterations;
    
    // 2 * N^3 operations per multiplication (multiply + add)
    double ops = 2.0 * size * size * size;
    double gflops = (ops / avg_time) / 1e9;

    std::cout << "Size: " << std::setw(4) << size << "x" << size 
              << " | Time: " << std::setw(8) << std::fixed << std::setprecision(4) << avg_time << " s"
              << " | GFLOPS: " << std::setw(6) << std::fixed << std::setprecision(2) << gflops 
              << std::endl;
}

int main() {
    std::cout << "Benchmarking Naive Matrix Multiplication:" << std::endl;
    
    // Test cases: Size, Iterations
    // Using smaller sizes/counts initially to keep it quick
    run_benchmark(128, 10);
    run_benchmark(256, 5);
    run_benchmark(512, 2); 
    
    return 0;
}
