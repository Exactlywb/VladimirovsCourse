#include "matrix.hpp"

int main () {


    //*** TESTS FOR MOVE STUFF ***
    // Math::Matrix<double> mainMatrix {5};
    // for (int i = 0; i < 5; ++i)
    //     for (int j = 0; j < 5; ++j)
    //         mainMatrix.setElemUsingRowCol (i, j, i * j);
    // mainMatrix.matrixDump ();

    // Math::Matrix<double> notSoMainMatrix {std::move (mainMatrix)};
    // notSoMainMatrix.matrixDump ();

    // Math::Matrix<double> thirdMatrix {5};
    // for (int i = 0; i < 5; ++i)
    //     for (int j = 0; j < 5; ++j)
    //         thirdMatrix.setElemUsingRowCol (i, j, (i + 1) * (j + 1));

    // thirdMatrix.matrixDump ();
    // notSoMainMatrix = thirdMatrix;
    // notSoMainMatrix.matrixDump ();

    int n = 0;
    std::cin >> n;

    Math::Matrix<double> mainMatrix {n};

    for (int i = 0; i < n * n; ++i)
        std::cin >> mainMatrix [i];

    std::cout << mainMatrix.calcDet () << std::endl;

    return 0;

}
