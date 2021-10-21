#ifndef OCTREE_HPP__
#define OCTREE_HPP__

#include <iostream>
#include <cmath>
#include <list>
#include "triangle.hpp"

namespace Tree {
    using pType = double;

    const double LittleSpace = 0.1;

    class Octree {

    private:
        GObjects::Vector rightBorder_, leftBorder_;	
        
        int whatChapter (GObjects::Vector &leftBorder, GObjects::Vector &rightBorder, const GObjects::Triangle &tr);
        void createNewNode (Octree &curRoot, int chapter);
        void siftTree(Octree &curRoot);

//-----------------------------------------------------------------------------------------------------

    public:
        using ListIt = typename std::list < GObjects::Triangle > ::iterator;
        Octree *child_[8] {};

        std::list < GObjects::Triangle > listOfTriangles_;

        Octree (GObjects::Vector right = 0, GObjects::Vector left = 0):
                rightBorder_ (right),
                leftBorder_ (left),
                child_ {} {}
        ~Octree ();

        void fillTree(int countTriangles);

        void dumpTree (Octree &curRoot);
    };
}

#endif
