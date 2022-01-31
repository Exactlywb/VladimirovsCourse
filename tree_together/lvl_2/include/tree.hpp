#ifndef TREE_H__
#define TREE_H__

#include <string.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>

namespace TreeImpl {

    template <typename T = int>
    struct BinNode {
        BinNode *left_ = nullptr;
        BinNode *right_ = nullptr;

        T val_{};

        BinNode () = default;
        BinNode (T val) : val_ (val) {}

        virtual ~BinNode () = default;
    };

    template <typename T = int>
    class SplayTree final {
        struct SplayNode final : public BinNode<T> {
            using BinNode<T>::left_;
            using BinNode<T>::right_;
            using BinNode<T>::val_;

            enum class AddSide { LEFT, RIGHT };

            SplayNode *parent_ = nullptr;

            SplayNode () = default;
            SplayNode (T val) : BinNode<T> (val) {}

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
                    << val_ << "</font>>]";
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
                if (curNode->left_)                                        //
                    curNode = static_cast<SplayNode *> (curNode->left_);   // here we go down by our tree
                else if (curNode->right_)                                  //
                    curNode = static_cast<SplayNode *> (curNode->right_);  //
                else if (curNode->parent_ && curNode != highestNode) {     // if we have papa &&
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
            MyIterator (SplayNode *ptr) : ptr_ (ptr) {}

            MyIterator (const MyIterator &rhs) : ptr_ (rhs.ptr) {}

            MyIterator operator= (const MyIterator &rhs)
            {
                ptr_ = rhs.ptr_;
                return *this;
            }
            MyIterator (MyIterator &&rhs) = delete;
            MyIterator operator= (MyIterator &&rhs) = delete;

            ~MyIterator () = default;

            SplayNode *operator* () const
            {
                return ptr_;
            }

            SplayNode *operator-> ()
            {
                return ptr_;
            }

            MyIterator &operator++ () {}

            MyIterator operator++ (int) {}

            MyIterator &operator-- () {}

            MyIterator operator-- (int) {}
        };

        //-----------------------------------------------------------------------------------------------------

        MyIterator begin ()
        {
            SplayNode *tmp = root_;
            while (tmp->left_)
                tmp = static_cast<SplayNode *> (tmp->left_);
            return MyIterator (tmp);
        }

        //-----------------------------------------------------------------------------------------------------

        MyIterator find (T val)
        {
            SplayNode *tmp =  root_; 

            while (tmp) {

                if (tmp->val_ > val) {

                    tmp = static_cast<SplayNode *> (tmp->left_);
                    if (tmp && tmp->val_ < val)
                        return MyIterator (nullptr);
                }
                else if (tmp->val_ < val) {

                    tmp = static_cast<SplayNode *> (tmp->right_);
                    if (tmp && tmp->val_ > val)
                        return MyIterator (nullptr);
                }
                else if (tmp->val_ == val)
                    return MyIterator (tmp);
            }

            return MyIterator (nullptr);
        }

        //-----------------------------------------------------------------------------------------------------

        MyIterator end() 
        {
            return MyIterator (nullptr);
        }

        //-----------------------------------------------------------------------------------------------------

        void rotateLeft (SplayNode *node)
        {
            SplayNode *parent = node->parent_;
            SplayNode *right = static_cast<SplayNode *> (node->right_);

            if (parent) {
                if (parent->left_ == node)
                    parent->left_ = right;
                else
                    parent->right_ = right;
            }

            SplayNode *tmp = static_cast<SplayNode *> (right->left_);
            right->left_ = node;
            node->right_ = tmp;
            node->parent_ = right;
            right->parent_ = parent;

            if (node->right_)
                static_cast<SplayNode *> (node->right_)->parent_ = node;
        }

        void rotateRight (SplayNode *node)
        {
            SplayNode *parent = node->parent_;
            SplayNode *left = static_cast<SplayNode *> (node->left_);

            if (parent) {
                if (parent->left_ == node)
                    parent->left_ = left;
                else
                    parent->right_ = left;
            }

            SplayNode *tmp = static_cast<SplayNode *> (left->right_);
            left->right_ = node;
            node->left_ = tmp;
            node->parent_ = left;
            left->parent_ = parent;

            if (node->left_)
                static_cast<SplayNode *> (node->left_)->parent_ = node;
        }

        void splay (SplayNode *node)
        {
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

                if (val == tmp->val_) {
                    splay (tmp);
                    root_ = tmp;
                    return;
                }

                if (val < tmp->val_)
                    tmp = static_cast<SplayNode *> (tmp->left_);
                else
                    tmp = static_cast<SplayNode *> (tmp->right_);
            }

            node->parent_ = curNode;
            if (curNode == nullptr)
                root_ = node;
            else if (val < curNode->val_)
                curNode->left_ = node;
            else
                curNode->right_ = node;

            splay (node);

            root_ = node;
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

}  // namespace TreeImpl

#endif
