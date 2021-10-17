#include <iostream>
#include "include/octotree.hpp"

int main()
{
	int countTriangles;
	// std::cin >> countTriangles;

	// Tree::Octotree <double> tree;

	// tree.fillTree(countTriangles);

	GObjects::Vector<double> vec_1 {1.5, 8.6, 65.5};
	GObjects::Vector<double> vec_2 {32, 65, 23};
	GObjects::Vector<double> vec_3 {923, 12, 1};

	// std::cout << "Please work!!! ---> " << GObjects::determinant (vec_1, vec_2, vec_3) << std::endl;

    GObjects::Vector<double> begin_1 {1, 1, 0};
    GObjects::Vector<double> begin_2 {1, 3, 0};
    GObjects::Vector<double> segment_1 {2, 2, 0};
    GObjects::Vector<double> segment_2 {2, -2, 0};

    GObjects::Triangle<double> tr_1{{2, -2, 0}, {0, 3, 0}, {2, 3, 0}};
    GObjects::Triangle<double> tr_2{{1, 1, 0}, {5, 1, 0}, {1, 5, 0}};

    std::cout << tr_1.intersectOnePlaneTriangle(tr_2) << std::endl;

	// GObjects::intersectSegments (begin_1, segment_1, begin_2, segment_2);	


	return 0;
}
