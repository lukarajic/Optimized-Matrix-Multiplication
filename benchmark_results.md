# Benchmark Results

Run on Feb 12 2026 at 11:46:09

### Matrix Size: 128x128

| Optimization | GFLOPS | Time (s) |
| :--- | :--- | :--- |
| Naive (i-j-k) | 1.21 | 0.0035 |
| Opt V1 (i-k-j) | 17.49 | 0.0002 |
| Opt V2 (Tiled) | 8.25 | 0.0005 |
| Opt V3 (Unroll) | 18.99 | 0.0002 |
| Opt V4 (SIMD) | 7.10 | 0.0006 |
| Opt V5 (Thread) | 30.11 | 0.0001 |
| Opt V6 (RegBlk) | 10.29 | 0.0004 |
| Opt V7 (Thrd+RegBlk) | 22.18 | 0.0002 |
| Opt V8 (Prefetch) | 3.34 | 0.0013 |
| Opt V9 (Transp) | 4.02 | 0.0010 |

### Matrix Size: 256x256

| Optimization | GFLOPS | Time (s) |
| :--- | :--- | :--- |
| Naive (i-j-k) | 2.20 | 0.0153 |
| Opt V1 (i-k-j) | 26.06 | 0.0013 |
| Opt V2 (Tiled) | 14.23 | 0.0024 |
| Opt V3 (Unroll) | 25.93 | 0.0013 |
| Opt V4 (SIMD) | 10.35 | 0.0032 |
| Opt V5 (Thread) | 81.75 | 0.0004 |
| Opt V6 (RegBlk) | 14.11 | 0.0024 |
| Opt V7 (Thrd+RegBlk) | 48.31 | 0.0007 |
| Opt V8 (Prefetch) | 4.27 | 0.0079 |
| Opt V9 (Transp) | 3.77 | 0.0089 |

### Matrix Size: 512x512

| Optimization | GFLOPS | Time (s) |
| :--- | :--- | :--- |
| Naive (i-j-k) | 1.85 | 0.1448 |
| Opt V1 (i-k-j) | 26.39 | 0.0102 |
| Opt V2 (Tiled) | 13.99 | 0.0192 |
| Opt V3 (Unroll) | 26.28 | 0.0102 |
| Opt V4 (SIMD) | 10.83 | 0.0248 |
| Opt V5 (Thread) | 108.83 | 0.0025 |
| Opt V6 (RegBlk) | 14.89 | 0.0180 |
| Opt V7 (Thrd+RegBlk) | 47.81 | 0.0056 |
| Opt V8 (Prefetch) | 4.38 | 0.0613 |
| Opt V9 (Transp) | 2.84 | 0.0945 |

### Matrix Size: 1024x1024

| Optimization | GFLOPS | Time (s) |
| :--- | :--- | :--- |
| Naive (i-j-k) | 1.70 | 1.2664 |
| Opt V1 (i-k-j) | 25.65 | 0.0837 |
| Opt V2 (Tiled) | 12.86 | 0.1670 |
| Opt V3 (Unroll) | 25.65 | 0.0837 |
| Opt V4 (SIMD) | 10.91 | 0.1968 |
| Opt V5 (Thread) | 119.66 | 0.0179 |
| Opt V6 (RegBlk) | 15.11 | 0.1421 |
| Opt V7 (Thrd+RegBlk) | 67.89 | 0.0316 |
| Opt V8 (Prefetch) | 4.38 | 0.4908 |
| Opt V9 (Transp) | 2.41 | 0.8900 |

