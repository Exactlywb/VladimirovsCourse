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

        int leftSize    = 0;
        int rightSize   = 0;

        Node ()         = default; //
        ~Node ()        = default; //!TODO discuss about it with Ivan and Vlad

    };

    struct Tree {
    
    private:
        Node* root = nullptr;
    public:

        Tree () = default;
        
        Tree (int val) {

            root = new Node {};
            root->val_ = val;

        }

        Tree (const Node* node) = delete;   //!TODO discuss about it with Ivan and Vlad.
                                            //It doesn't look good to hand over external ptr.

        ~Tree ();

        void    graphDump       (const char* fileName);
        int     push            (int val);      //!TODO it has to return err I think.

        int     getNLessThan    (int border);

    };

}

#endif
