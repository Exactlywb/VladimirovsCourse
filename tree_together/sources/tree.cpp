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

namespace {

    void PrintNodes (std::ofstream& dumpOut, const TreeImpl::Node* curNode) {
        if (curNode == nullptr)
            return;
        static int curNodeNum = 0;
        dumpOut << curNodeNum << "[label = \"" << curNode->val_ << "\"]\n";

    }

//-----------------------------------------------------------------------------------------------------

    void rightRotate () {

    }

//-----------------------------------------------------------------------------------------------------

    void leftRotate () {

    }

//-----------------------------------------------------------------------------------------------------

    void balanceTree () {

    }
}

void TreeImpl::Tree::graphDump (const char* fileName) {

    std::ofstream dumpOut (fileName, dumpOut.out | dumpOut.trunc);

    dumpOut << "digraph Tree {\n";

    PrintNodes (dumpOut, root);

    dumpOut << "}";

}

int TreeImpl::Tree::push (int val) {

    TreeImpl::Node* newNode = new TreeImpl::Node {val};
    TreeImpl::Node* tmpNode = root,
                  * tmpParent = nullptr;

    while (tmpNode != nullptr) {  //big data in right child

        tmpParent = tmpNode;
        if (tmpNode->val_ < val)
            tmpNode = tmpNode->right_;
        else
            tmpNode = tmpNode->left_;
    }

    newNode->parent_ = tmpParent;
    newNode->left_ = newNode->right_ = nullptr;

    if (tmpParent == nullptr) {

        root = newNode;
        return 0;
    }

    if (tmpParent->val_ < val)
        tmpParent->right_ = newNode;
    else
        tmpParent->left_ = newNode;

    newNode->color = 1;

    balanceTree();

    return -1; //!TODO remake this func

}

int TreeImpl::Tree::getNLessThan (int border) {

    //!TODO

    return 0;

}
