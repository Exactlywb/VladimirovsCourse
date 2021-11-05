#include "matrix.hpp"

int main () {

    int n = 0;
    std::cin >> n;

    Math::Matrix<int> mainMatrix {n};
    //std::cin >> mainMatrix [n - 1];
    //std::cout << mainMatrix [n - 1];
    // for (int i = 0; i < 2 * n; i++)
    //     std::cin >> mainMatrix [n];
    
    // std::cout << mainMatrix.calcDet ();

    return 0;

}
