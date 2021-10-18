#include "../include/octree.hpp"

void checkUnder (Tree::Octree *octree, GObjects::Triangle &tr, int &underCounter) {

    for (int i  = 0; i < 8; ++i) {
        if (!octree->child_[i])
            return;

        std::list < GObjects::Triangle > List = octree->child_[i]->listOfTriangles_;
        
        for (Tree::Octree::ListIt It = List.begin(); It != List.end(); ++It) {
            underCounter += GObjects::Intersect3DTriangles (tr, *It);
        }

        checkUnder (octree->child_[i], tr, underCounter);
    }

}

static int IntersectionCounter (Tree::Octree *octree) {

    static int counter = 0;

    if (octree == nullptr)
        return counter;

    std::list <GObjects::Triangle> List = octree->listOfTriangles_;
    
    for (Tree::Octree::ListIt ItSlow = List.begin(); ItSlow != List.end(); ++ItSlow) {
        Tree::Octree::ListIt curIt = ItSlow;
        ++curIt;
        for (Tree::Octree::ListIt ItFast = curIt; ItFast != List.end(); ++ItFast) {
            counter += GObjects::Intersect3DTriangles (*ItSlow, *ItFast);
        }
        
        checkUnder (octree, *ItSlow, counter);

    }
    return counter;
}

int GetTriangles () {

    int countTriangles;
    std::cin >> countTriangles;
    assert(std::cin.good());

    Tree::Octree octree{};
    octree.fillTree(countTriangles); 

    return IntersectionCounter (&octree);
}   