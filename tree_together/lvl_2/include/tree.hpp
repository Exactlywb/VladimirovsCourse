#ifndef TREE_H__
#define TREE_H__

#include <string.h>

#include <cassert>
#include <fstream>
#include <iostream>

namespace TreeImpl {

template <typename T = int>
struct BinNode {

    BinNode *left_  = nullptr;
    BinNode *right_ = nullptr;

    T val_ {};

    BinNode () = default;
    BinNode (T val): val_ (val) {}

    virtual ~BinNode () = default;
};

template <typename T = int>
class SplayTree final {

    struct SplayNode final: public BinNode<T> {
        using BinNode<T>::left_;
        using BinNode<T>::right_;
        using BinNode<T>::val_;

        int subtreeSize_    = 1;
        SplayNode* parent_  = nullptr;

        SplayNode () = default;
        SplayNode (T val, SplayNode* parent): BinNode<T>(val), parent_ (parent) {}

        ~SplayNode () = default;

        SplayNode (const SplayNode &other) = delete;                //
        SplayNode (SplayNode &&other) = delete;                     //
                                                                    // We are not lazy. We've discussed about this.
        SplayNode &operator= (const SplayNode &other) = delete;     //
        SplayNode &operator= (SplayNode &&other) = delete;          //

        void disactiveChild(const SplayNode *child)
        {
            if (left_ == child)
                left_ = nullptr;
            else if (right_ == child)
                right_ = nullptr;
        }

        void graphDump (std::ofstream& out) const {
            std::cout << "here1" << std::endl;
            std::cout << "this = " << this << std::endl;
            out << "\"box" << this << "\" [shape = \"record\", color = \"white\" label = <<font color = \"#242424\">" 
                << val_ << ", sizeSubtree = " << subtreeSize_ << "</font>>]";
std::cout << "here2" << std::endl;
            if (left_ != nullptr) {

                static_cast<SplayNode*>(left_)->graphDump (out);
                out << "\"box" << this << "\" -> \"box" << left_ << "\"" << std::endl;
            }

            if (right_ != nullptr) {

                static_cast<SplayNode*>(right_)->graphDump (out);
                out << "\"box" << this << "\" -> \"box" << right_ << "\"" << std::endl;
            }
            
        }
    };

    //-----------------------------------------------------------------------------------------------------

    SplayNode *root_ = nullptr;

    public:
    SplayTree() = default;

    ~SplayTree()
    {
        SplayNode *highestNode = root_;
        SplayNode *curNode = root_;
        SplayNode *toDelete = nullptr;

        while (curNode) {
            if (curNode->left_)                                             //
                curNode = static_cast<SplayNode *> (curNode->left_);        // here we go down by our tree
            else if (curNode->right_)                                       //
                curNode = static_cast<SplayNode *> (curNode->right_);       //
            else if (curNode->parent_ && curNode != highestNode) {          // if we have papa &&
                                                                            // we are not in the highest point

                toDelete = curNode;
                curNode = curNode->parent_;

                curNode->disactiveChild(toDelete);

                delete toDelete;
            }
            else {
                SplayNode *parent = curNode->parent_;
                if (parent)
                    parent->disactiveChild(curNode);

                delete curNode;
                return;
            }
        }
    }

    SplayTree(const SplayTree<T> &other)                 // copy constructor
    {
        if (other.root_ == nullptr) {
            root_ = nullptr;
            return;
        }

        root_ = new SplayNode;
        assert(root_);

        SplayNode *curCopy = root_;
        SplayNode *curOther = other.root_;

        while ((curOther->left_ || curOther->right_) || root_->subtreeSize != other.root_->subtreeSize) {
            if (curCopy->left_ == nullptr && curOther->left_) {  // didn't concat the left node of other subtree

                curCopy->left_ = new SplayNode;

                curCopy->left_->parent_ = curCopy;

                curCopy = curCopy->left_;
                curOther = curOther->left_;
            }
            else if (curCopy->right_ == nullptr && curOther->right_) {  // didn't concat the right node of other subtree

                curCopy->right_ = new SplayNode;

                curCopy->right_->parent_ = curCopy;

                curCopy = curCopy->right_;
                curOther = curOther->right_;
            }
            else {
                curCopy->subtreeSize = curOther->subtreeSize;

                curCopy->val_ = curOther->val_;
                curCopy->color_ = curOther->color_;

                if (curCopy != root_) {
                    curCopy = curCopy->parent_;
                    curOther = curOther->parent_;
                }
                else
                    break;
            }
        }
    }

    SplayTree &operator=(const SplayTree<T> &other) //copy assignment
    {
        if (this == &other)  // same pointers -_-
            return *this;

        SplayTree tempTree{other};

        *this = std::move(tempTree);
        return *this;
    }

    SplayTree(SplayTree<T> &&other)                 // move constructor
    {                                                       
        root_ = other.root_;        //
        other.root_ = nullptr;      // stolen -_-
    }

    SplayTree &operator=(SplayTree<T> &&other)      // move assignment
    {                                                       

        if (this == &other)  // same ptr
            return *this;

        root_->deleteSubtree();     //
        root_ = other.root_;        // stolen -_-
        other.root_ = nullptr;      //

        return *this;
    }

    void insert (SplayNode* node) {

        SplayNode* curNode = NULL;
        

    }

    void graphDump (const char* fileName) const {

        std::ofstream dumpOut(fileName, dumpOut.out | dumpOut.trunc);
        dumpOut << "digraph Tree {\n";

        if (root_)
            root_->graphDump (dumpOut);

        dumpOut << "}";

    }
};

}

#endif
