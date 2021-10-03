#include "triangles.hpp"

int main () {

    Geometry::Point<double> firstPoint (1, 1, 1);
    Geometry::Point<double> secondPoint (1, 2, 3);

    Geometry::Point<double> newPoint = firstPoint + secondPoint - firstPoint;

    std::cin >> newPoint;
    std::cout << newPoint << std::endl;

    //std::cout << newPoint;

    return 0;

}
