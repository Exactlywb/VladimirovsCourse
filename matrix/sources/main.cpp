#include <iostream>
#include "matrix.hpp"

int main () { 

    int size = 0;
    std::cin >> size;

    Matrix::Matrix<double> matr (size, size);

    for (int y = 0; y < size; ++y) {

        for (int x = 0; x < size; ++x)
            std::cin >> matr [y] [x];

    }

    matr.textDump ();

    std::cout << matr.det () << std::endl;

}

