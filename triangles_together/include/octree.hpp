#ifndef OCTREE_HPP__
#define OCTREE_HPP__

#include <iostream>
#include <cmath>
#include <list>
#include <vector>
#include "triangle.hpp"

namespace Tree {
    using pType = double;

    const double LittleSpace = 0.1;

    class OctreeNode final{

    private:
        GObjects::Vector rightBorder_, leftBorder_;
        OctreeNode *parent_ {};
        
        int whatChapter (GObjects::Vector &leftBorder, GObjects::Vector &rightBorder, const GObjects::Triangle &tr) noexcept;
        void createNewNode (OctreeNode &curRoot, int chapter);
        void siftTree(OctreeNode &curRoot);

        void disactiveChild (OctreeNode* curChild) {
            
            for (int i = 0; i < 8; ++i) {

                if (curChild == child_[i]) {
                    
                    child_[i] = nullptr;
                    break;
                }
            }   

        }

//-----------------------------------------------------------------------------------------------------

    public:
        using ListIt = typename std::list < GObjects::Triangle > ::iterator;
        
        OctreeNode *child_[8] {};
        std::list < GObjects::Triangle > listOfTriangles_;

//-----------------------------------------------------------------------------------------------------

        OctreeNode (GObjects::Vector right = 0, GObjects::Vector left = 0):
                rightBorder_ (right),
                leftBorder_ (left),
                child_ {} {}

//-----------------------------------------------------------------------------------------------------

        OctreeNode (const OctreeNode &rhs)            = delete;         // ban copy ctor
        OctreeNode (OctreeNode &&rhs)                 = delete;         // ban move ctor
        OctreeNode &operator =(OctreeNode &&rhs)      = delete;         // ban move assignment
        OctreeNode &operator =(const OctreeNode &rhs) = delete;         // ban assignment operator
        ~OctreeNode ()                            = default; 
//-----------------------------------------------------------------------------------------------------

        void deleteSubtree () {       // TODO: Belov must make normal destructor))0               

            OctreeNode* mainRoot = this;
            OctreeNode* curNode  = this;
            OctreeNode* toDelete = nullptr;

            while (curNode) {
                
                bool checker = false;
                for (int i = 0; i < 8; ++i) {
                    
                    if (curNode->child_[i]) {
                        
                        checker = true;
                    
                        curNode = curNode->child_[i];
                        break;
                    }
                }   

                if (checker)  {
                    continue;             
                }
                else if (curNode->parent_ && curNode != mainRoot) {   
                                                                     
                    toDelete    = curNode;
                    curNode     = curNode->parent_;

                    curNode->disactiveChild (toDelete);
                    
                    delete toDelete;

                } else {

                    OctreeNode* parent = curNode->parent_;
                    if (parent)
                        parent->disactiveChild (curNode);
                        
                    delete curNode;
                    return; 

                }

            }
            
        }

//-----------------------------------------------------------------------------------------------------

        void fillTree   (const std::vector<GObjects::Triangle>& triangles);
        void dumpTree   (OctreeNode &curRoot);

    };

    class Octree final {

        private:
        OctreeNode* root = nullptr;

    public:
        Octree () : root(new OctreeNode) {};

        ~Octree () { root->deleteSubtree (); }    

        Octree (const Octree& other) = delete;                  //copy constructor
        Octree (Octree&& other)      = delete;                  //move constructor
        
        Octree& operator= (const Octree& other) = delete;       //copy assignment
        Octree& operator= (Octree&& other)      = delete;       //move assignment

        void    push  (const std::vector<GObjects::Triangle>& triangles) {

            root->fillTree (triangles);

        }
        
        OctreeNode* getRoot  () const noexcept { return root; }
        

    };
}

#endif
