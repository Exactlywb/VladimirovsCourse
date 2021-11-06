#ifndef TREE_H__
#define TREE_H__

#include <iostream>
#include <string.h>
#include <fstream>

namespace TreeImpl {

    enum color {
        BLACK = 0,
        RED = 1
    };

    struct Node {

        Node* left_     = nullptr;
        Node* right_    = nullptr;

        Node* parent_   = nullptr;

        int val_        = 0;

        bool color_ = BLACK;

        int subtreeSize    = 1; //!TODO add it into the copy / move

        Node    (int val = 0, Node* parent = nullptr):
                val_ (val),
                parent_ (parent) {}
        
        ~Node () = default;

        Node (const Node& other)    = delete;               //
        Node (Node&& other)         = delete;               //
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
        Node*   copySubtree     (); // |!| WARNING: This function copy the object itself.

    };

    struct Tree {
    
    private:
        Node* root = nullptr;

        void rightRotate    (Node *x);
        void leftRotate     (Node *x);
        void balanceTree    (Node *pushedNode);

    public:

        Tree () = default;

        Tree (const Node* node) = delete;   //It doesn't look good to hand over external ptr.
        //!TODO rule of 5
        ~Tree ();

        Tree (const Tree& other);           //copy constructor
        Tree (Tree&& other);                //move constructor
        
        Tree& operator= (const Tree& other);    //copy assignment
        Tree& operator= (Tree&& other);         //move assignment

        void    graphDump       (const char* fileName);
        void    push            (int val);

        int     getNLessThan    (int border);

    };

}

#endif
