#include "tree.hpp"

namespace TreeImpl {


    Tree::~Tree () {

        root->deleteSubtree ();

    }

    Tree::Tree (const Tree& other) {

        root = new Node;

        Node* curCopy   = root;
        Node* curOther  = other.root;

        Node* copyHighest   = root;

        while (curCopy->left_ && curCopy->right_ && curCopy != copyHighest) {

            if (curCopy->left_ == nullptr && curOther->left_) { //didn't concat the left node of other subtree

                curCopy->left_ = new Node;
                
                curCopy->left_->parent_ = curCopy;

                curCopy     = curCopy->left_;
                curOther    = curOther->left_;

            } else if (curCopy->right_ == nullptr && curOther->right_) { //didn't concat the right node of other subtree

                curCopy->right_ = new Node;
                
                curCopy->right_->parent_ = curCopy;

                curCopy     = curCopy->right_;
                curOther    = curOther->right_;

            } else {

                curCopy->subtreeSize = curOther->subtreeSize;

                curCopy->val_ = curOther->val_;

                if (curCopy != copyHighest) {

                    curCopy     = curCopy->parent_;
                    curOther    = curOther->parent_;

                }
            }
        }
    }

    Tree& Tree::operator=  (const Tree& other) {
        
        if (this == &other)     //same pointers -_-
            return *this;

        Tree tempTree {other};

        *this = std::move (tempTree);

        return *this;

    }

    Tree::Tree (Tree&& other) { //move constructor

        root = other.root;      //
        other.root = nullptr;   //stolen -_-
        
    }

    Tree& Tree::operator= (Tree&& other) { //move assignment

        if (this == &other)     //same ptr
            return *this;

        root->deleteSubtree (); //
        root = other.root;      //stolen -_-
        other.root = nullptr;   //

        return *this;

    }

    // |!| WARNING: This function delete the object itself.
    void Node::deleteSubtree () {

        //Generally speaking, the use of the Node class may 
        //not be in one implementation of the tree, 
        //so it is best to leave this method in the Node class.

        Node* highestNode   = this;
        Node* curNode       = this;
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

        void PrintNodes (std::ofstream& dumpOut, const Node* curNode) {
            if (curNode == nullptr)
                return;
            
            static int curNodeNum = 0;
            int curInd = curNodeNum;
            
            if (curNode->color_ == BLACK) {
                dumpOut << "\"box"  << curNodeNum << "\" [shape = \"record\", color = \"white\" label = <<font color = \"#242424\">" 
                                    << curNode->val_ << ", sizeSubtree = " << curNode->subtreeSize << "</font>>]" << std::endl;
            }
            else if (curNode->color_ == RED) {
                dumpOut << "\"box" << curNodeNum << "\" [shape = \"record\", color=\"white\", label = <<font color = \"#c2453c\">" 
                        << curNode->val_ << ", sizeSubtree = " << curNode->subtreeSize << "</font>>]" << std::endl;
            }
            else {

                std::cout << "Unexpected color in function " << __func__ << std::endl;
                return; 

            }

            curNodeNum++;

            if (curNode->left_ != nullptr) {

                size_t nextIndex = curNodeNum;
                PrintNodes (dumpOut, curNode->left_);
                dumpOut << "\"box" << curInd << "\" -> \"box"<< nextIndex << "\"" << std::endl;

            }

            if (curNode->right_ != nullptr) {

                size_t nextIndex = curNodeNum;
                PrintNodes (dumpOut, curNode->right_);
                dumpOut << "\"box" << curInd << "\" -> \"box"<< nextIndex << "\"" << std::endl;

            }

        }

    }

    //=====================================================================================================

    void Tree::graphDump (const char* fileName) {

        std::ofstream dumpOut (fileName, dumpOut.out | dumpOut.trunc);

        dumpOut << "digraph Tree {\n";

        PrintNodes (dumpOut, root);

        dumpOut << "}";

    }

    //-----------------------------------------------------------------------------------------------------

    void Tree::rightRotate (Node *x) {

        Node *y = x->left_;

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

    void Tree::leftRotate (Node *x) {

        Node *y = x->right_;

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

    void Tree::balanceTree (Node *curNode) {

        while (curNode->parent_ != nullptr && curNode->parent_->color_ == RED) {

            Node *granddad = curNode->parent_->parent_;

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

    void Tree::push (int val) {

        Node* newNode = new Node {val};
        Node* tmpNode = root,
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

    int Tree::KthOrderStatistic (int key) { 

        Node* curNode = root;
        int rang = 1;
        
        while (curNode) {

            if (curNode->left_)
                rang = curNode->left_->subtreeSize + 1;
            else  
                rang = 1;   


            if (key == rang)
                return curNode->val_;

            if (key < rang) {
                curNode = curNode->left_; 
            }
            else {
                curNode = curNode->right_;
                key -= rang;
            }
        }
        return -666;
    }

    //-----------------------------------------------------------------------------------------------------

    int Tree::getNLessThan (int border) {

        Node* curNode = root;
        int amount{};

        while (curNode) {

            if (border > curNode->val_) {

                if (curNode->left_) 
                    amount += curNode->left_->subtreeSize + 1;
                else
                    ++amount;

                curNode = curNode->right_;
            }
            else if (border <= curNode->val_) 
                curNode = curNode->left_;
        }

        return amount;
    }
}