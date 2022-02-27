#ifndef TREE_H__
#define TREE_H__

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <algorithm>
#include <cassert>
#include <complex>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <vector>

namespace TreeImpl {

    template <typename T = int>
    class SplayTree final {
        struct SplayNode final {
            SplayNode *left_;
            SplayNode *right_;
            T val_;

            enum class AddSide { LEFT, RIGHT };

            SplayNode *parent_ = nullptr;
            int subtreeSize = 1;

            SplayNode () = default;
            SplayNode (T val) : val_ (val) {}

            ~SplayNode () = default;

            SplayNode (const SplayNode &other) = delete;             //
            SplayNode (SplayNode &&other) = delete;                  //
                                                                     // We are not lazy. We've discussed about this.
            SplayNode &operator= (const SplayNode &other) = delete;  //
            SplayNode &operator= (SplayNode &&other) = delete;       //

            void disactiveChild (const SplayNode *child)
            {
                if (left_ == child)
                    left_ = nullptr;
                else if (right_ == child)
                    right_ = nullptr;
            }

            void graphDump (std::ofstream &out) const
            {
                out << "\"box" << this
                    << "\" [shape = \"record\", color = \"white\" label = <<font color = "
                       "\"#242424\">"
                    << val_ << " subtreeSize = " << subtreeSize << "</font>>]";
                if (left_ != nullptr) {
                    static_cast<SplayNode *> (left_)->graphDump (out);
                    out << "\"box" << this << "\" -> \"box" << left_ << "\"" << std::endl;
                }

                if (right_ != nullptr) {
                    static_cast<SplayNode *> (right_)->graphDump (out);
                    out << "\"box" << this << "\" -> \"box" << right_ << "\"" << std::endl;
                }
            }

            void addChild (SplayTree *child, AddSide side)
            {
                child->parent_ = this;
                if (side == AddSide::LEFT)
                    left_ = child;
                else
                    right_ = child;
            }
        };

        //-----------------------------------------------------------------------------------------------------

        SplayNode *root_ = nullptr;

    public:
        SplayTree () = default;

        ~SplayTree ()
        {
            SplayNode *highestNode = root_;
            SplayNode *curNode = root_;
            SplayNode *toDelete = nullptr;

            while (curNode) {
                if (curNode->left_)                                     //
                    curNode = curNode->left_;                           // here we go down by our tree
                else if (curNode->right_)                               //
                    curNode = curNode->right_;                          //
                else if (curNode->parent_ && curNode != highestNode) {  // if we have papa &&
                    // we are not in the highest point

                    toDelete = curNode;
                    curNode = curNode->parent_;

                    curNode->disactiveChild (toDelete);

                    delete toDelete;
                }
                else {
                    SplayNode *parent = curNode->parent_;
                    if (parent)
                        parent->disactiveChild (curNode);

                    delete curNode;
                    return;
                }
            }
        }

        SplayTree (const SplayTree<T> &other)  // copy constructor
        {
            if (other.root_ == nullptr) {
                root_ = nullptr;
                return;
            }

            root_ = new SplayNode;
            assert (root_);

            SplayNode *curCopy = root_;
            SplayNode *curOther = other.root_;

            while ((curOther->left_ || curOther->right_) || root_->subtreeSize != other.root_->subtreeSize) {
                if (curCopy->left_ == nullptr && curOther->left_) {  // didn't concat the left node of other subtree

                    curCopy->left_ = new SplayNode;

                    curCopy->left_->parent_ = curCopy;

                    curCopy = curCopy->left_;
                    curOther = curOther->left_;
                }
                else if (curCopy->right_ == nullptr && curOther->right_) {  // didn't concat the right node of other
                    // subtree

                    curCopy->right_ = new SplayNode;

                    curCopy->right_->parent_ = curCopy;

                    curCopy = curCopy->right_;
                    curOther = curOther->right_;
                }
                else {
                    curCopy->subtreeSize = curOther->subtreeSize;

                    curCopy->val_ = curOther->val_;

                    if (curCopy != root_) {
                        curCopy = curCopy->parent_;
                        curOther = curOther->parent_;
                    }
                    else
                        break;
                }
            }
        }

        SplayTree &operator= (const SplayTree<T> &other)  // copy assignment
        {
            if (this == &other)  // same pointers -_-
                return *this;

            SplayTree tempTree{other};

            *this = std::move (tempTree);
            return *this;
        }

        SplayTree (SplayTree<T> &&other)  // move constructor
        {
            root_ = other.root_;    //
            other.root_ = nullptr;  // stolen -_-
        }

        SplayTree &operator= (SplayTree<T> &&other)  // move assignment
        {
            if (this == &other)  // same ptr
                return *this;

            root_->deleteSubtree ();  //
            root_ = other.root_;      // stolen -_-
            other.root_ = nullptr;    //

            return *this;
        }

        //-----------------------------------------------------------------------------------------------------

        class MyIterator final : public std::iterator<std::bidirectional_iterator_tag, SplayNode *> {
            SplayNode *ptr_;

        public:
            MyIterator (SplayNode *ptr = nullptr) : ptr_ (ptr) {}

            MyIterator (const MyIterator &rhs) : ptr_ (rhs.ptr_) {}

            MyIterator operator= (const MyIterator &rhs)
            {
                ptr_ = rhs.ptr_;
                return *this;
            }

            MyIterator (MyIterator &&rhs) : ptr_ (rhs.ptr_) {}
            MyIterator operator= (MyIterator &&rhs)
            {
                std::swap (ptr_, rhs.ptr_);
                return *this;
            }

            ~MyIterator () = default;

            SplayNode *operator* () const
            {
                return ptr_;
            }

            SplayNode *operator-> ()
            {
                return ptr_;
            }

            MyIterator &operator++ ()
            {
                if (ptr_->right_) {
                    ptr_ = ptr_->right_;
                    while (ptr_->left_)
                        ptr_ = ptr_->left_;

                    *this = MyIterator (ptr_);
                    return *this;
                }

                SplayNode *tmp = ptr_;

                while (tmp->parent_ && tmp->parent_->left_ != tmp)
                    tmp = tmp->parent_;

                *this = MyIterator (tmp->parent_);
                return *this;
            }

            MyIterator operator++ (int)
            {
                MyIterator *tmp = *this;
                ++(*this);
                return tmp;
            }

            MyIterator &operator-- ()
            {
                if (ptr_->left_) {
                    ptr_ = ptr_->left_;

                    while (ptr_->right_)
                        ptr_ = ptr_->right_;
                    *this = MyIterator (ptr_);
                    return *this;
                }

                SplayNode *tmp = ptr_;

                while (tmp->parent_ && tmp->parent_->right_ != tmp)
                    tmp = tmp->parent_;

                *this = MyIterator (tmp->parent_);
                return *this;
            }

            MyIterator operator-- (int)
            {
                MyIterator *tmp = *this;
                --(*this);
                return tmp;
            }

            bool equal (const MyIterator &rhs) const
            {
                return ptr_ == rhs.ptr_;
            }
        };

        //-----------------------------------------------------------------------------------------------------

        MyIterator begin ()
        {
            SplayNode *tmp = root_;
            while (tmp->left_)
                tmp = tmp->left_;
            return MyIterator (tmp);
        }

        //-----------------------------------------------------------------------------------------------------

        MyIterator find (T val)
        {
            SplayNode *tmp = root_;

            while (tmp) {
                if (tmp->val_ > val) {
                    tmp = tmp->left_;
                }
                else if (tmp->val_ < val) {
                    tmp = tmp->right_;
                }
                else if (tmp->val_ == val) {
                    splay (tmp);
                    return MyIterator (tmp);
                }
            }

            return MyIterator (nullptr);
        }

        //-----------------------------------------------------------------------------------------------------

        MyIterator end ()
        {
            return MyIterator (nullptr);
        }

        //-----------------------------------------------------------------------------------------------------

        MyIterator upperBound (T val)
        {
            SplayNode *curNode = root_;
            SplayNode *tmp = nullptr;

            while (curNode) {
                if (curNode->val_ == val) {
                    curNode = curNode->right_;
                    continue;
                }
                if (curNode->val_ < val) {
                    curNode = curNode->right_;
                }
                else {
                    tmp = curNode;
                    curNode = curNode->left_;
                }
            }
            splay (tmp);

            return MyIterator (tmp);
        }

        //-----------------------------------------------------------------------------------------------------

        MyIterator lowerBound (T val)
        {
            SplayNode *curNode = root_;
            SplayNode *tmp = nullptr;

            while (curNode) {
                if (curNode->val_ == val) {
                    splay (curNode);
                    return MyIterator (curNode);
                }

                if (curNode->val_ > val) {
                    tmp = curNode;
                    curNode = curNode->left_;
                }
                else {
                    curNode = curNode->right_;
                }
            }
            splay (tmp);

            return MyIterator (tmp);
        }

        //-----------------------------------------------------------------------------------------------------

        long getNLessThan (T border)
        {
            SplayNode *curNode = root_;
            long amount{};

            while (curNode) {
                if (border > curNode->val_) {
                    if (curNode->left_)
                        amount += curNode->left_->subtreeSize + 1;
                    else
                        ++amount;

                    curNode = curNode->right_;
                }
                else
                    curNode = curNode->left_;
            }
            return amount;
        }

        //-----------------------------------------------------------------------------------------------------

        long range (T left, T right)
        {
            auto leftIt = upperBound (left);
            auto rightIt = lowerBound (right);

            return getNLessThan ((*rightIt)->val_) - getNLessThan ((*leftIt)->val_);
        }

        //-----------------------------------------------------------------------------------------------------
        void rotateLeft (SplayNode *node)
        {
            SplayNode *parent = node->parent_;
            SplayNode *right = node->right_;

            right->subtreeSize = node->subtreeSize;
            int leftSize = (right->left_) ? right->left_->subtreeSize : 0, rightSize = (node->left_) ? node->left_->subtreeSize : 0;
            node->subtreeSize = leftSize + rightSize + 1;

            if (parent) {
                if (parent->left_ == node)
                    parent->left_ = right;
                else
                    parent->right_ = right;
            }

            SplayNode *tmp = right->left_;
            right->left_ = node;
            node->right_ = tmp;
            node->parent_ = right;
            right->parent_ = parent;

            if (node->right_)
                node->right_->parent_ = node;
        }

        void rotateRight (SplayNode *node)
        {
            SplayNode *parent = node->parent_;
            SplayNode *left = node->left_;

            left->subtreeSize = node->subtreeSize;
            int leftSize = (left->right_) ? left->right_->subtreeSize : 0, rightSize = (node->right_) ? node->right_->subtreeSize : 0;
            node->subtreeSize = leftSize + rightSize + 1;

            if (parent) {
                if (parent->left_ == node)
                    parent->left_ = left;
                else
                    parent->right_ = left;
            }

            SplayNode *tmp = left->right_;
            left->right_ = node;
            node->left_ = tmp;
            node->parent_ = left;
            left->parent_ = parent;

            if (node->left_)
                node->left_->parent_ = node;
        }

        void splay (SplayNode *node)
        {
            if (!node)
                return;

            while (node->parent_) {
                SplayNode *parent = node->parent_;
                SplayNode *grandParent = parent->parent_;

                if (node == parent->left_) {
                    if (!grandParent) {
                        rotateRight (parent);
                    }
                    else {
                        if (parent == grandParent->left_) {
                            rotateRight (grandParent);
                            rotateRight (node->parent_);
                        }
                        else {
                            rotateRight (parent);
                            rotateLeft (node->parent_);
                        }
                    }
                }
                else {
                    if (!grandParent) {
                        rotateLeft (parent);
                    }
                    else {
                        if (parent == grandParent->right_) {
                            rotateLeft (grandParent);
                            rotateLeft (node->parent_);
                        }
                        else {
                            rotateLeft (parent);
                            rotateRight (node->parent_);
                        }
                    }
                }
            }

            root_ = node;
        }

        SplayNode *getRoot () const
        {
            return root_;
        }

        void insert (T val)
        {
            SplayNode *node = new SplayNode (val);

            SplayNode *curNode = nullptr;
            SplayNode *tmp = root_;

            while (tmp != nullptr) {
                curNode = tmp;
                ++curNode->subtreeSize;

                if (val == tmp->val_) {
                    while (curNode->parent_) {
                        --curNode->subtreeSize;
                        curNode = curNode->parent_;
                    }
                    --curNode->subtreeSize;
                    splay (tmp);
                    return;
                }

                if (val < tmp->val_)
                    tmp = tmp->left_;
                else
                    tmp = tmp->right_;
            }

            node->parent_ = curNode;
            if (curNode == nullptr)
                root_ = node;
            else if (val < curNode->val_)
                curNode->left_ = node;
            else
                curNode->right_ = node;
        }

        void graphDump (const char *fileName) const
        {
            std::ofstream dumpOut (fileName, dumpOut.out | dumpOut.trunc);
            dumpOut << "digraph Tree {\n";

            if (root_)
                root_->graphDump (dumpOut);

            dumpOut << "}";
        }
    };

    template <typename T = int>
    bool operator== (const typename SplayTree<T>::MyIterator &lhs, const typename SplayTree<T>::MyIterator &rhs)
    {
        return lhs.equal (rhs);
    }

    template <typename T = int>
    bool operator!= (const typename SplayTree<T>::MyIterator &lhs, const typename SplayTree<T>::MyIterator &rhs)
    {
        return !lhs.equal (rhs);
    }

}  // namespace TreeImpl

#endif
