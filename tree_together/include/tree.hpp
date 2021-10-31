#ifndef TREE_H__
#define TREE_H__

#include <iostream>
#include <string.h>
#include <fstream>

namespace TreeImpl {

    struct Node {

        Node* left_     = nullptr;
        Node* right_    = nullptr;

        Node* parent_   = nullptr;

        int val_        = 0;

        bool color = 0;         //0 - black
                                //1 - red

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
        Node* root = nullptr;

    public:

        Tree () = default;

        Tree (const Node* node) = delete;   //!TODO discuss about it with Ivan and Vlad.
                                            //It doesn't look good to hand over external ptr.

        ~Tree ();

        void    graphDump       (const char* fileName);
        int     push            (int val);      //!TODO it has to return err I think.

        int     getNLessThan    (int border);

    };

}

#endif
