#ifndef LINEAR_ALGEBRA_H
#define LINEAR_ALGEBRA_H

#include <vector>
#include <stdexcept>
#include <stdio.h>

class LinearAlgebra {
public:
    std::vector<double> multiply(
        const std::vector<std::vector<double>>& A, const std::vector<double>& x
    );
};


#endif 