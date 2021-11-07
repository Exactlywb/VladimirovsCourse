#include "matrix.hpp"

int main () {

    int n = 0;
    std::cin >> n;

    Math::Matrix<double> mainMatrix {n};
    
    for (int i = 0; i < n * n; ++i)
        std::cin >> mainMatrix [i];

    mainMatrix.matrixDump ();

    std::cout << "det = " << mainMatrix.calcDet () << std::endl;

    return 0;

}
