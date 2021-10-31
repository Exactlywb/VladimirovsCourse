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

        int subtreeSize    = 0;

        Node    (int val = 0, Node* parent = nullptr):
                val_ (val),
                parent_ (parent) {}
        ~Node ()        = default; //!TODO discuss about it with Ivan and Vlad

        void disactiveChild (const Node* child) {

            if (left_ == child)
                left_ = nullptr;
            else if (right_ == child)
                right_ = nullptr;

        }

    };

    struct Tree {
    
    private:


        void rightRotate (Node *x);
        void leftRotate (Node *x);
        void balanceTree (Node *pushedNode);

    public:
        Node* root = nullptr;

        Tree () = default;

        Tree (const Node* node) = delete;   //!TODO discuss about it with Ivan and Vlad.
                                            //It doesn't look good to hand over external ptr.

        ~Tree ();

        void    graphDump       (const char* fileName);
        void    push            (int val);

        int     getNLessThan    (int border);

    };

}

#endif
