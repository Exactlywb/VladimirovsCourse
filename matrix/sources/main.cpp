#include "matrix.hpp"

int main () {

    int n = 0;
    std::cin >> n;

    Math::Matrix<int> mainMatrix {n};
    int val = 12;
    mainMatrix.SetElemUsingRowNumAndColNum (1, 1, val);
    mainMatrix.MatrixDump ();

    return 0;

}
