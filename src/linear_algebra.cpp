#include "linear_algebra.h"

std::vector<double> LinearAlgebra::multiply(
    const std::vector<std::vector<double>>& A,
    const std::vector<double>& x)
{
    if (A.empty()) {
        throw std::invalid_argument("Empty matrix");
    }

    size_t cols = A[0].size();

    for (const auto& row : A) {
        if (row.size() != cols) {
            throw std::invalid_argument("Inconsistent matrix row size");
        }
    }

    if (cols != x.size()) {
        throw std::invalid_argument("Dimension mismatch");
    }

    std::vector<double> y(A.size(), 0.0);

    for (size_t i = 0; i < A.size(); i++) {
        double sum = 0.0;

        for (size_t j = 0; j < cols; j++) {
            sum += A[i][j] * x[j];
        }

        y[i] = sum;
    }

    return y;
}