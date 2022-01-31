#ifndef TREE_H__
#define TREE_H__

#include <string.h>

#include <cassert>
#include <fstream>
#include <iostream>

namespace TreeImpl {

template <typename T = int>
class BinNode {

    BinNode *left_  = nullptr;
    BinNode *right_ = nullptr;

    T val_ {};

public:
    BinNode () = default;
    BinNode (T val): val_ (val) {}

    virtual ~BinNode () = 0;

    void disactiveChild(const Node *child)
    {
        if (left_ == child)
            left_ = nullptr;
        else if (right_ == child)
            right_ = nullptr;
    }

    void deleteSubtree () 
    { // |!| WARNING: This function delete the object itself.

        // Generally speaking, the use of the Node class may
        // not be in one implementation of the tree,
        // so it is best to leave this method in the Node class.

        BinNode *highestNode = this;
        BinNode *curNode = this;
        BinNode *toDelete = nullptr;

        while (curNode) {
            if (curNode->left_)                                     //
                curNode = curNode->left_;                           // here we go down by our tree
            else if (curNode->right_)                               //
                curNode = curNode->right_;                          //
            else if (curNode->parent_ && curNode != highestNode) {  // if we have papa &&
                                                                    // we are not in the highest point

                toDelete = curNode;
                curNode = curNode->parent_;

                curNode->disactiveChild(toDelete);

                delete toDelete;
            }
            else {
                BinNode *parent = curNode->parent_;
                if (parent)
                    parent->disactiveChild(curNode);

                delete curNode;
                return;
            }
        }

    }

};

template <typename T = int>
class SplayTree final {

    class SplayNode final: public BinNode<T> {

        int subtreeSize_    = 1;
        SplayNode* parent_  = nullptr;

    public:
        SplayNode () = default;
        SplayNode (T val, SplayNode* parent): val_ (val), parent_ (parent) {}

        ~SplayNode () = default;

        SplayNode (const SplayNode &other) = delete;                //
        SplayNode (SplayNode &&other) = delete;                     //
                                                                    // We are not lazy. We've discussed about this.
        SplayNode &operator= (const SplayNode &other) = delete;     //
        SplayNode &operator= (SplayNode &&other) = delete;          //

        void graphDump (const std::ofstream& out) const {
            
            out << "\"box" << this << "\" [shape = \"record\", color = \"white\" label = <<font color = \"#242424\">" 
                << val_ << ", sizeSubtree = " << subtreeSize_   << "</font>>]" << std::endl;

            if (curNode->left_ != nullptr) {

                left_.graphDump (out);
                out << "\"box" << this << "\" -> \"box" << left_ << "\"" << std::endl;
            }

            if (curNode->right_ != nullptr) {

                right_.graphDump (out);
                out << "\"box" << this << "\" -> \"box" << right_ << "\"" << std::endl;
            }
            
        }

    };

    SplayNode *root_ = nullptr;

    public:
    SplayTree() = default;

    ~SplayTree()
    {
        root_->deleteSubtree();
    }

    SplayTree::SplayTree(const Tree &other)                 // copy constructor
    {
        if (other.root == nullptr) {
            root = nullptr;
            return;
        }

        root = new SplayNode<T>;
        assert(root);

        SplayNode<T> *curCopy = root;
        SplayNode<T> *curOther = other.root;

        while ((curOther->left_ || curOther->right_) || root->subtreeSize != other.root->subtreeSize) {
            if (curCopy->left_ == nullptr && curOther->left_) {  // didn't concat the left node of other subtree

                curCopy->left_ = new SplayNode<T>;

                curCopy->left_->parent_ = curCopy;

                curCopy = curCopy->left_;
                curOther = curOther->left_;
            }
            else if (curCopy->right_ == nullptr && curOther->right_) {  // didn't concat the right node of other subtree

                curCopy->right_ = new SplayNode<T>;

                curCopy->right_->parent_ = curCopy;

                curCopy = curCopy->right_;
                curOther = curOther->right_;
            }
            else {
                curCopy->subtreeSize = curOther->subtreeSize;

                curCopy->val_ = curOther->val_;
                curCopy->color_ = curOther->color_;

                if (curCopy != root) {
                    curCopy = curCopy->parent_;
                    curOther = curOther->parent_;
                }
                else
                    break;
            }
        }
    }

    SplayTree &SplayTree::operator=(const SplayTree &other) //copy assignment
    {
        if (this == &other)  // same pointers -_-
            return *this;

        SplayTree tempTree{other};

        *this = std::move(tempTree);
        return *this;
    }

    SplayTree::SplayTree(SplayTree &&other)                 // move constructor
    {                                                       

        root = other.root;     //
        other.root = nullptr;  // stolen -_-
    }

    SplayTree &SplayTree::operator=(SplayTree &&other)      // move assignment
    {                                                       

        if (this == &other)  // same ptr
            return *this;

        root->deleteSubtree();  //
        root = other.root;      // stolen -_-
        other.root = nullptr;   //

        return *this;
    }

    void graphDump (const char* fileName) const {

        std::ofstream dumpOut(fileName, dumpOut.out | dumpOut.trunc);
        dumpOut << "digraph Tree {\n";

        root.graphDump (dumpOut);

        dumpOut << "}";

    }

    SplayNode* getRoot const () { return root_; }

};

}

#endif TREE_H__
