#include "octree.hpp"
#include "vulkan.hpp"

namespace {

    void checkUnder (Tree::OctreeNode *octree, GObjects::Triangle &tr, bool *intersectTriangleFlagArray, int &underCounter) {

        for (int i  = 0; i < 8; ++i) {
            if (!octree->child_[i])
                continue;
            
            std::list < GObjects::Triangle > List = octree->child_[i]->listOfTriangles_;
            
            for (Tree::OctreeNode::ListIt It = List.begin(); It != List.end(); ++It) {
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

//-----------------------------------------------------------------------------------------------------

    int IntersectionCounter (Tree::OctreeNode *octree, bool *intersectTriangleFlagArray) {

        int counter = 0;

        if (octree == nullptr)
            return counter;

        std::list <GObjects::Triangle> List = octree->listOfTriangles_;
        
        for (Tree::OctreeNode::ListIt ItSlow = List.begin(); ItSlow != List.end(); ++ItSlow) {

            Tree::OctreeNode::ListIt curIt = ItSlow;
            ++curIt;
            for (Tree::OctreeNode::ListIt ItFast = curIt; ItFast != List.end(); ++ItFast) {
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

            if (!(octree->child_ [i]))
                continue;

            counter += IntersectionCounter (octree->child_ [i], intersectTriangleFlagArray);

        }

        return counter;

    }
}

//=====================================================================================================

int GetTriangles () {

    int countTriangles = 0;
    std::cin >> countTriangles;
    assert (std::cin.good());

    Tree::Octree mainRoot {};

    std::vector<GObjects::Triangle> trianglesArr;
    GObjects::Triangle tr;
    for (int i = 0; i < countTriangles; ++i) {

        std::cin >> tr;
        trianglesArr.push_back (tr);

    }

    mainRoot.push (trianglesArr);

    bool *intersectTriangleFlagArray = new bool [countTriangles] {};

    int countIntersection = IntersectionCounter (mainRoot.getRoot(), intersectTriangleFlagArray);

    for (int i = 0; i < countTriangles; ++i) {
        if (intersectTriangleFlagArray[i])
            std::cout << i << std::endl;
    }

    drawTriangles (trianglesArr, intersectTriangleFlagArray);
    
    delete [] intersectTriangleFlagArray;

    return countIntersection;
}
