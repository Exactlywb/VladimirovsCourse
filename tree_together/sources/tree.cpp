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

//=====================================================================================================

namespace {

    void PrintNodes (std::ofstream& dumpOut, const TreeImpl::Node* curNode) {
        if (curNode == nullptr)
            return;
        static int curNodeNum = 0;
        dumpOut << curNodeNum << "[label = \"" << curNode->val_ << "\"]\n";

    }

}

//=====================================================================================================

void TreeImpl::Tree::graphDump (const char* fileName) {

    std::ofstream dumpOut (fileName, dumpOut.out | dumpOut.trunc);

    dumpOut << "digraph Tree {\n";

    PrintNodes (dumpOut, root);

    dumpOut << "}";

}

//-----------------------------------------------------------------------------------------------------

void TreeImpl::Tree::rightRotate (TreeImpl::Node *x) {

    TreeImpl::Node *y = x->left_;

    y->subtreeSize = x->subtreeSize;
    int leftSize  = (y->right_) ? y->right_->subtreeSize : 0,
        rightSize = (x->right_) ? x->right_->subtreeSize : 0;
    x->subtreeSize = leftSize + rightSize + 1;

    x->left_ = y->right_;

    if (y->right_ != nullptr)
        y->right_->parent_ = x;

    y->parent_ = x->parent_;

    if(y->parent_ == nullptr) 
        root = y;
    else if (y->parent_->left_ == x)
        y->parent_->left_ = y;
    else 
        y->parent_->right_ = y;

    y->right_ = x;
    x->parent_ = y;

}

//-----------------------------------------------------------------------------------------------------

void TreeImpl::Tree::leftRotate (TreeImpl::Node *x) {

    TreeImpl::Node *y = x->right_;

    y->subtreeSize = x->subtreeSize;
    int leftSize  = (y->left_) ? y->left_->subtreeSize : 0,
        rightSize = (x->left_) ? x->left_->subtreeSize : 0;
    x->subtreeSize = leftSize + rightSize + 1;

    x->right_ = y->left_;

    if (y->left_ != nullptr)
        y->left_->parent_ = x;

    y->parent_ = x->parent_;

    if(y->parent_ == nullptr) 
        root = y;
    else if (y->parent_->left_ == x)
        y->parent_->left_ = y;
    else 
        y->parent_->right_ = y;

    y->left_ = x;
    x->parent_ = y;
}

//-----------------------------------------------------------------------------------------------------

void TreeImpl::Tree::balanceTree (TreeImpl::Node *curNode) {

    while (curNode->parent_ != nullptr && curNode->parent_->color_ == RED) {

        TreeImpl::Node *granddad = curNode->parent_->parent_;

        if (granddad->left_ == curNode->parent_) {

            if (granddad->right_ != nullptr && granddad->right_->color_ == RED) {

                granddad->right_->color_ = BLACK;
                curNode->parent_->color_ = BLACK;
                granddad->color_ = RED;

                curNode = granddad;
            }
            else {
                
                if (curNode->parent_->right_ == curNode) {
                
                    curNode = curNode->parent_;
                    leftRotate (curNode);
                }

                granddad->color_ = RED;
                curNode->parent_->color_ = BLACK;

                rightRotate (granddad);
            }
        }
        else {

            if (granddad->left_ != nullptr && granddad->left_->color_ == RED) {

                granddad->left_->color_ = BLACK;
                curNode->parent_->color_ = BLACK;
                granddad->color_ = RED;

                curNode = granddad;
            }
            else {
                
                if (curNode->parent_->left_ == curNode) {
                
                    curNode = curNode->parent_;
                    rightRotate (curNode);
                }

                granddad->color_ = RED;
                curNode->parent_->color_ = BLACK;

                leftRotate (granddad);
            }
        }
    }

    root->color_ = BLACK;
}

//-----------------------------------------------------------------------------------------------------

void TreeImpl::Tree::push (int val) {

    TreeImpl::Node* newNode = new TreeImpl::Node {val};
    TreeImpl::Node* tmpNode = root,
                  * tmpParent = nullptr;

    while (tmpNode != nullptr) {  //big data in right child

        tmpParent = tmpNode;
        ++tmpParent->subtreeSize;

        if (tmpNode->val_ < val)
            tmpNode = tmpNode->right_;
        else
            tmpNode = tmpNode->left_;
    }

    newNode->parent_ = tmpParent;
    newNode->left_ = newNode->right_ = nullptr;

    if (tmpParent == nullptr) {

        root = newNode;
        return;
    }

    if (tmpParent->val_ < val)
        tmpParent->right_ = newNode;
    else
        tmpParent->left_ = newNode;

    newNode->color_ = RED;

    balanceTree(newNode);
}

//-----------------------------------------------------------------------------------------------------

int TreeImpl::Tree::getNLessThan (int border) {

    //!TODO

    return 0;

}