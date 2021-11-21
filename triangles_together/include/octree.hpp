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
        Octree *parent {};
        
        int whatChapter (GObjects::Vector &leftBorder, GObjects::Vector &rightBorder, const GObjects::Triangle &tr);
        void createNewNode (Octree &curRoot, int chapter);
        void siftTree(Octree &curRoot);

//-----------------------------------------------------------------------------------------------------

    public:
        using ListIt = typename std::list < GObjects::Triangle > ::iterator;
        
        Octree *child_[8] {};
        std::list < GObjects::Triangle > listOfTriangles_;

//-----------------------------------------------------------------------------------------------------

        Octree (GObjects::Vector right = 0, GObjects::Vector left = 0):
                rightBorder_ (right),
                leftBorder_ (left),
                child_ {} {}

//-----------------------------------------------------------------------------------------------------

        Octree (const Octree &rhs) = delete;            // ban copy ctor
        Octree (Octree &&rhs) = delete;                 // ban move ctor
        Octree &operator =(Octree &&rhs) = delete;      // ban move assignment
        Octree &operator =(const Octree &rhs) = delete; // ban assignment operator

//-----------------------------------------------------------------------------------------------------

        ~Octree () {        // TODO: Frolov must make normal destructor))0
            for(int i = 0; i < 8; ++i)
                delete child_[i];
        }

//-----------------------------------------------------------------------------------------------------

        void fillTree(int countTriangles);
        void dumpTree (Octree &curRoot);
    };
}

#endif
