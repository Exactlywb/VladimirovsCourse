#include "tree.hpp"

TreeImpl::Tree::~Tree () {

    //!TODO this destructor is bad cause he deletes not only subtree
    //but the whole tree.

    Node* curNode   = root;
    Node* toDelete  = nullptr;

    while (curNode) {

        if (curNode->left_)             //
            curNode = curNode->left_;   //here we go down by our tree
        else if (curNode->right_)       //
            curNode = curNode->right_;  //
        else if (curNode->parent_) {    //if we have papa

            toDelete    = curNode;
            curNode     = curNode->parent_;

            if (curNode->left_ == toDelete)
                curNode->left_ = nullptr;
            else if (curNode->right_ == toDelete)
                curNode->right_ = nullptr;
            
            delete toDelete;

        } else {
            
            delete curNode;
            break;

        }

    }

}

void TreeImpl::Tree::graphDump (const char* fileName) {

    //!TODO

}

int TreeImpl::Tree::push (int val) {

    //!TODO

    return -1;

}

int TreeImpl::Tree::getNLessThan (int border) {

    //!TODO

    return 0;

}
