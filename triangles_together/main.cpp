#include <iostream>
#include "include/octotree.hpp"

int main()
{
//	int countTriangles;
//	std::cin >> countTriangles;

//	Tree::Octotree tree;

//	tree.fillTree(countTriangles);

    GObjects::Triangle tr1 ({1, 0, 0}, {0, 1, 0}, {-1, 0, 0});
    std::cout << tr1 << std::endl;

    GObjects::Triangle tr2 ({0, 0, 0}, {0, 0, 0}, {0, 0, 0});
    std::cout << tr2 << std::endl;

    bool test = GObjects::Intersect3DTriangles (tr1, tr2);
    if (test)
        std::cout << "Intersects" << std::endl;
    else
        std::cout << "Not intersects :(" << std::endl;

    return 0;
}

