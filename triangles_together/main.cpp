#include <iostream>
#include "include/octotree.hpp"

int main()
{
// int countTriangles;
// std::cin >> countTriangles;

// Tree::Octotree tree;

// tree.fillTree(countTriangles);

    GObjects::Triangle tr1 {};
    GObjects::Triangle tr2 {};
    std::cin >> tr1 >> tr2;
    assert (std::cin.good());

    std::cout << tr1;
    std::cout << tr2;
    
    bool test = GObjects::Intersect3DTriangles (tr1, tr2);
    if (test)
        std::cout << "Intersect" << std::endl;
    else
        std::cout << "Not :(" << std::endl;

    return 0;
}

