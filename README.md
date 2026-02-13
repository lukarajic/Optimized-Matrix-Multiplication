# Optimized Matrix Multiplication

![Language](https://img.shields.io/badge/language-C%2B%2B17-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

This project explores high-performance computing techniques by incrementally optimizing matrix multiplication on the CPU. Starting from a naive triple-loop implementation (~1.7 GFLOPS), the project applies layers of optimization—including cache blocking, SIMD vectorization, and multi-threading—to achieve a peak performance of **~120 GFLOPS** on Apple Silicon (M-series), a **~70x speedup**.

The project serves as a practical exploration of computer architecture concepts, focusing on memory hierarchy, instruction level parallelism (ILP), and thread-level parallelism (TLP).

## Key Skills Demonstrated

*   **C++17 Systems Programming:** Manual memory management, aligned allocation, pointer arithmetic.
*   **Computer Architecture:** Cache locality (spatial/temporal), false sharing, hardware prefetching.
*   **Vectorization (SIMD):** ARM NEON intrinsics (with cross-platform AVX2 scaffolding).
*   **Concurrency:** Multi-threading with `std::thread`, workload distribution, race condition avoidance.
*   **Performance Analysis:** Benchmarking, GFLOPS measurement, analyzing compiler auto-vectorization vs. manual intrinsics.

## Optimization Journey

The project implements and benchmarks the following versions:

1.  **Naive (i-j-k):** Baseline implementation. Poor cache locality.
2.  **Loop Reordering (i-k-j):** **[Big Win]** Accesses memory linearly. Huge speedup due to spatial locality.
3.  **Tiling (Blocking):** Improves temporal locality for large matrices.
4.  **Loop Unrolling:** Reduces branch overhead (though modern compilers often automate this).
5.  **SIMD (NEON/AVX):** Manual vectorization using intrinsics. *Interesting finding: On M-series chips, compiler auto-vectorization on simple loops often outperforms basic manual intrinsics.*
6.  **Multi-threading:** **[Massive Win]** Uses `std::thread` to parallelize the outer loop, scaling linearly with core count.
7.  **Register Blocking:** Increases arithmetic intensity.
8.  **Software Prefetching:** Uses `__builtin_prefetch`. *Result: Negative impact on this architecture, likely interfering with the hardware prefetcher.*
9.  **Matrix Transposition:** Transposes Matrix B to allow dot-product access. *Result: Memory copy overhead outweighed the access pattern benefits for tested sizes.*

## Performance Results (1024x1024 Matrix)

| Optimization | GFLOPS | Speedup vs Naive | Notes |
| :--- | :--- | :--- | :--- |
| **Naive (i-j-k)** | 1.70 | 1.0x | Baseline |
| **Opt V1 (i-k-j)** | 25.65 | 15.1x | **Best single-thread scalar** |
| **Opt V2 (Tiled)** | 12.86 | 7.6x | Overhead > Benefit at this size |
| **Opt V3 (Unroll)** | 25.65 | 15.1x | Compiler already unrolling V1 |
| **Opt V4 (SIMD)** | 10.91 | 6.4x | Manual intrinsics slower than auto-vec |
| **Opt V5 (Thread)** | **119.66** | **70.4x** | **Peak Performance** |
| **Opt V7 (Thread+RegBlk)**| 67.89 | 39.9x | Complex kernel hindered compiler |
| **Opt V9 (Transp)** | 2.41 | 1.4x | Copy overhead too high |

## Build and Run

### Prerequisites
*   C++17 compliant compiler (GCC/Clang)
*   Make

### Compilation
```bash
make
```

### Running Tests
Verifies correctness of all optimized kernels against the naive baseline.
```bash
make test
```

### Running Benchmarks
Runs performance benchmarks for all versions and outputs to `benchmark_results.md`.
```bash
make && ./bin/benchmark
```

## Hardware Context
*   **Architecture:** ARM64 (Apple Silicon)
*   **Compiler:** Apple Clang 17.0.0 (optimization flags: `-O3`)
