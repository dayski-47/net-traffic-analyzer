#ifndef MATRIX_BUILDER_H
#define MATRIX_BUILDER_H

#include <vector>
#include <stdio.h>
#include "windowing.h"


class MatrixBuilder {
public:
    std::vector<std::vector<double>> buildMatrix(const std::vector<Window>& windows);
};



#endif