#ifndef TREE_H__
#define TREE_H__

#include <iostream>
#include <string.h>
#include <fstream>
#include <cassert>

namespace TreeImpl {

    enum Color {
        BLACK = 0,
        RED = 1
    };

    struct Node final {

        Node* left_     = nullptr;
        Node* right_    = nullptr;

        Node* parent_   = nullptr;

        int val_        = 0;

        bool color_     = BLACK;

        int subtreeSize    = 1;

        Node    (int val = 0, Node* parent = nullptr):
                val_ (val),
                parent_ (parent) {}
        
        ~Node () = default; // default != {}

        Node (const Node& other)            = delete;               //
        Node (Node&& other)                 = delete;               //
                                                            // We are not lazy. We've discussed about this.
        Node& operator= (const Node& other) = delete;       //
        Node& operator= (Node&& other)      = delete;       //

        void disactiveChild (const Node* child) {

            if (left_ == child)
                left_ = nullptr;
            else if (right_ == child)
                right_ = nullptr;

        }

        void    deleteSubtree   (); // |!| WARNING: This function delete the object itself.

    };

    class Tree final {
    
    private:
        Node* root = nullptr;

        enum childType {
            LEFT,
            RIGHT
        };

        void rightRotate    (Node *x);
        void leftRotate     (Node *x);
        void insertFixUp    (Node *pushedNode);
        Node *partialFixUp  (Node *curNode, enum childType side);

    public:
        Tree () = default;

        ~Tree () { root->deleteSubtree (); }    

        Tree (const Tree& other);           //copy constructor
        Tree (Tree&& other);                //move constructor
        
        Tree& operator= (const Tree& other);    //copy assignment
        Tree& operator= (Tree&& other);         //move assignment

        void    graphDump         (const char* fileName);
        void    push              (int val);

        int     getNLessThan      (int border);
        int     KthOrderStatistic (int key);

        Node*   getRoot           () const { return root; }
        
    };

}

#endif
