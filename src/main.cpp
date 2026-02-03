#include <iostream>
#include "matrix.h"

int main() {
    size_t size = 4;
    Matrix A(size, size);
    Matrix B(size, size);
    Matrix C(size, size);

    // Initialize simple data
    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < size; ++j) {
            A(i, j) = 1.0f;
            B(i, j) = 2.0f;
        }
    }

    try {
        multiply_naive(A, B, C);
        std::cout << "Multiplication successful." << std::endl;
        std::cout << "C(0,0) = " << C(0, 0) << " (Expected: 8.0)" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
