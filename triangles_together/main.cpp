#include <iostream>
#include "include/octotree.hpp"

int main()
{
//	int countTriangles;
//	std::cin >> countTriangles;

//	Tree::Octotree tree;

//	tree.fillTree(countTriangles);

    GObjects::Triangle tr1 {{0,0,1}, {-1,0,-1}, {1,0,-1}};
    std::cout << tr1 << std::endl;

    GObjects::Triangle tr2 {{0,0,-2}, {-1,0,1}, {1,0,-1}};
    std::cout << tr2 << std::endl;

	GObjects::Vector point {8,2,1};

    bool test = GObjects::Intersect2DTriangles(tr1, tr2);
    if (test)
        std::cout << "Intersect" << std::endl;
    else
        std::cout << "Not :(" << std::endl;

    return 0;
}

