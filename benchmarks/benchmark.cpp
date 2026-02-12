#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <fstream>
#include "matrix.h"

// Helper function to measure performance
void run_benchmark(size_t size, std::ofstream& out_file) {
    Matrix A(size, size);
    Matrix B(size, size);
    Matrix C(size, size);

    for (size_t i = 0; i < size * size; ++i) {
        A.data[i] = static_cast<float>(i);
        B.data[i] = static_cast<float>(i);
    }

    // Write table header for this size
    out_file << "### Matrix Size: " << size << "x" << size << "\n\n";
    out_file << "| Optimization | GFLOPS | Time (s) |\n";
    out_file << "| :--- | :--- | :--- |\n";

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

        // Console output
        std::cout << "  " << std::left << std::setw(20) << name 
                  << " | GFLOPS: " << std::fixed << std::setprecision(2) << gflops 
                  << " | Time: " << std::setprecision(4) << avg_time << "s" << std::endl;
        
        // File output
        out_file << "| " << name << " | " << std::fixed << std::setprecision(2) << gflops 
                 << " | " << std::setprecision(4) << avg_time << " |\n";
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
    benchmark_func([](const Matrix& A, const Matrix& B, Matrix& C) {
        multiply_optimized_v7_threaded_register_blocked(A, B, C);
    }, "Opt V7 (Thrd+RegBlk)");
    benchmark_func(multiply_optimized_v8_prefetch, "Opt V8 (Prefetch)");
    benchmark_func(multiply_optimized_v9_transpose, "Opt V9 (Transp)");
    
    out_file << "\n";
    std::cout << "--------------------------------------------------------" << std::endl;
}

int main() {
    std::cout << "Matrix Multiplication Benchmarks" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    
    std::ofstream out_file("benchmark_results.md");
    out_file << "# Benchmark Results\n\n";
    out_file << "Run on " << __DATE__ << " at " << __TIME__ << "\n\n";

    run_benchmark(128, out_file);
    run_benchmark(256, out_file);
    run_benchmark(512, out_file); 
    run_benchmark(1024, out_file);
    
    std::cout << "Results written to benchmark_results.md" << std::endl;
    return 0;
}
