#include "octree.hpp"
#include <vector>

static int IntersectionCounter (Tree::Octree *octree, bool *intersectTriangleFlagArray);

void checkUnder (Tree::Octree *octree, GObjects::Triangle &tr, bool *intersectTriangleFlagArray, int &underCounter) {

    for (int i  = 0; i < 8; ++i) {
        if (!octree->child_[i])
            continue;
        
        std::list < GObjects::Triangle > List = octree->child_[i]->listOfTriangles_;
        
        for (Tree::Octree::ListIt It = List.begin(); It != List.end(); ++It) {
            bool addUnderCounter = GObjects::Intersect3DTriangles (tr, *It);
            underCounter += addUnderCounter;

            if(addUnderCounter) {
                intersectTriangleFlagArray[It->getNumber()] = true;
                intersectTriangleFlagArray[tr.getNumber()] = true;
            }
        }

        checkUnder (octree->child_[i], tr, intersectTriangleFlagArray, underCounter);

    }

}

static int IntersectionCounter (Tree::Octree *octree, bool *intersectTriangleFlagArray) {

    int counter = 0;

    if (octree == nullptr)
        return counter;

    std::list <GObjects::Triangle> List = octree->listOfTriangles_;
    
    for (Tree::Octree::ListIt ItSlow = List.begin(); ItSlow != List.end(); ++ItSlow) {

        Tree::Octree::ListIt curIt = ItSlow;
        ++curIt;
        for (Tree::Octree::ListIt ItFast = curIt; ItFast != List.end(); ++ItFast) {
            bool addCounter = GObjects::Intersect3DTriangles (*ItSlow, *ItFast);
            counter += addCounter;

            if(addCounter) {
                intersectTriangleFlagArray[ItFast->getNumber()] = true;
                intersectTriangleFlagArray[ItSlow->getNumber()] = true;
            }
        }

        checkUnder (octree, *ItSlow, intersectTriangleFlagArray, counter);
        
    }

    for (int i = 0; i < 8; ++i) {

        if (!octree->child_ [i])
            continue;

        counter += IntersectionCounter (octree->child_ [i], intersectTriangleFlagArray);

        return counter;

    }

    return counter;

}

int GetTriangles () {

    int countTriangles;
    std::cin >> countTriangles;
    assert(std::cin.good());

    Tree::Octree octree{};
    octree.fillTree(countTriangles);

    bool *intersectTriangleFlagArray = new bool [countTriangles] {};

    int countIntersection = IntersectionCounter (&octree, intersectTriangleFlagArray);

    for (int i = 0; i < countTriangles; ++i) {
        if (intersectTriangleFlagArray[i])
            std::cout << i << std::endl;
    }
    
    delete [] intersectTriangleFlagArray;

    return countIntersection;
}