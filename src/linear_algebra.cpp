#include "linear_algebra.h"

std::vector<double> LinearAlgebra::multiply(
        const std::vector<std::vector<double>>& A, 
        const std::vector<double>& x){

    if (A.empty() || A[0].size() != x.size()) {
        throw std::invalid_argument("Dimension mismatch");
    }

    std::vector<double> y(A.size(), 0.0); 
    
    for (size_t i = 0; i < A.size(); 0.0){
        double sum = 0.0;

        for (size_t j = 0; j < x.size(); j++) {
            sum += A[i][j] * x[j];
        }

        y[i] = sum;
    }

    return y;

        


}