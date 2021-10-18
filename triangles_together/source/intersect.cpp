#include "../include/octotree.hpp"

static int IntersectionCounter (Tree::Octotree  *octree) {

    if (octree == nullptr)
        return;
}

int GetTriangles () {

    int countTriangles;
    std::cin >> countTriangles;
    assert(std::cin.good());

    Tree::Octotree octree{};
    octree.fillTree(countTriangles); 

    return IntersectionCounter (&octree);
}   