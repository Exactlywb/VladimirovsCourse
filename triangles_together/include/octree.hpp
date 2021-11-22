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
        Octree *parent_ {};
        
        int whatChapter (GObjects::Vector &leftBorder, GObjects::Vector &rightBorder, const GObjects::Triangle &tr);
        void createNewNode (Octree &curRoot, int chapter);
        void siftTree(Octree &curRoot);

        void disactiveChild (Octree* curChild) {
            
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
        
        Octree *child_[8] {};
        std::list < GObjects::Triangle > listOfTriangles_;

//-----------------------------------------------------------------------------------------------------

        Octree (GObjects::Vector right = 0, GObjects::Vector left = 0):
                rightBorder_ (right),
                leftBorder_ (left),
                child_ {} {}

//-----------------------------------------------------------------------------------------------------

        Octree (const Octree &rhs)            = delete;         // ban copy ctor
        Octree (Octree &&rhs)                 = delete;         // ban move ctor
        Octree &operator =(Octree &&rhs)      = delete;         // ban move assignment
        Octree &operator =(const Octree &rhs) = delete;         // ban assignment operator
        ~Octree ()                            = default; 
//-----------------------------------------------------------------------------------------------------

        void deleteSubtree () {       // TODO: Belov must make normal destructor))0               

            Octree* mainRoot = this;
            Octree* curNode  = this;
            Octree* toDelete = nullptr;

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

                    Octree* parent = curNode->parent_;
                    if (parent)
                        parent->disactiveChild (curNode);
                        
                    delete curNode;
                    return; 

                }

            }
            
        }

//-----------------------------------------------------------------------------------------------------

        void fillTree(int countTriangles);
        void dumpTree (Octree &curRoot);

    };

    class Tree final{

        private:
        Octree* root = nullptr;

    public:
        Tree () : root(new Octree) {};

        ~Tree () { root->deleteSubtree (); }    

        Tree (const Tree& other) = delete;           //copy constructor
        Tree (Tree&& other)      = delete;                //move constructor
        
        Tree& operator= (const Tree& other) = delete;    //copy assignment
        Tree& operator= (Tree&& other)      = delete;         //move assignment

        void    push  (int countTriangles) {

            root->fillTree (countTriangles);
        }
        Octree* getRoot  () const { return root; }
        

    };
}

#endif
