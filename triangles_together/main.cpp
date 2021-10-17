#include <iostream>
#include "include/octotree.hpp"

int main()
{
//	int countTriangles;
//	std::cin >> countTriangles;

//	Tree::Octotree tree;

//	tree.fillTree(countTriangles);

    GObjects::Triangle tr1 {{1,1,1}, {5,1,1}, {3,4,1}};
    std::cout << tr1 << std::endl;

    GObjects::Triangle tr2 {{8,4,1}, {7,1,1}, {9,1,1}};
    std::cout << tr2 << std::endl;

	GObjects::Vector point {8,2,1};

    bool test =  tr2.pointInTriangle(point);
    if (test)
        std::cout << "In" << std::endl;
    else
        std::cout << "Not :(" << std::endl;

    return 0;
}

