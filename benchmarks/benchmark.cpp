#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include "matrix.h"

// Helper function to measure performance
void run_benchmark(size_t size) {
    Matrix A(size, size);
    Matrix B(size, size);
    Matrix C(size, size);

    for (size_t i = 0; i < size * size; ++i) {
        A.data[i] = static_cast<float>(i);
        B.data[i] = static_cast<float>(i);
    }

    auto benchmark_func = [&](auto func, const std::string& name) {
        // Warmup
        func(A, B, C);
        
        auto start = std::chrono::high_resolution_clock::now();
        int iterations = (size <= 256) ? 10 : 2;
        for (int i = 0; i < iterations; ++i) {
            func(A, B, C);
        }
        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> elapsed = end - start;
        double avg_time = elapsed.count() / iterations;
        double ops = 2.0 * size * size * size;
        double gflops = (ops / avg_time) / 1e9;

        std::cout << "  " << std::left << std::setw(15) << name 
                  << " | GFLOPS: " << std::fixed << std::setprecision(2) << gflops 
                  << " | Time: " << std::setprecision(4) << avg_time << "s" << std::endl;
    };

    std::cout << "Matrix Size: " << size << "x" << size << std::endl;
    benchmark_func(multiply_naive, "Naive (i-j-k)");
    benchmark_func(multiply_optimized_v1, "Opt V1 (i-k-j)");
    benchmark_func([](const Matrix& A, const Matrix& B, Matrix& C) {
        multiply_optimized_v2_tiled(A, B, C, 32);
    }, "Opt V2 (Tiled)");
    benchmark_func(multiply_optimized_v3_unrolled, "Opt V3 (Unroll)");
    benchmark_func(multiply_optimized_v4_simd, "Opt V4 (SIMD)");
    benchmark_func([](const Matrix& A, const Matrix& B, Matrix& C) {
        multiply_optimized_v5_threaded(A, B, C);
    }, "Opt V5 (Thread)");
    benchmark_func(multiply_optimized_v6_register_blocked_2x2, "Opt V6 (RegBlk)");
    std::cout << "--------------------------------------------------------" << std::endl;
}

int main() {
    std::cout << "Matrix Multiplication Benchmarks" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    
    run_benchmark(128);
    run_benchmark(256);
    run_benchmark(512); 
    run_benchmark(1024);
    
    return 0;
}
