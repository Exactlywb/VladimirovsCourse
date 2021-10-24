#include "tree.hpp"

TreeImpl::Tree::~Tree () {
    
    Node* highestNode   = root;
    Node* curNode       = root;
    Node* toDelete      = nullptr;

    while (curNode) {

        if (curNode->left_)             //
            curNode = curNode->left_;   //here we go down by our tree
        else if (curNode->right_)       //
            curNode = curNode->right_;  //
        else if (curNode->parent_ && curNode != highestNode) {      //if we have papa && 
                                                                    //we are not in the highest point

            toDelete    = curNode;
            curNode     = curNode->parent_;

            curNode->disactiveChild (toDelete);
            
            delete toDelete;

        } else {

            Node* parent = curNode->parent_;
            if (parent)
                parent->disactiveChild (curNode);
                
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
