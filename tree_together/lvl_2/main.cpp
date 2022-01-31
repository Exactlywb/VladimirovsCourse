#include "include/tree.hpp"

int main()
{
    TreeImpl::SplayTree<int> tree;

    tree.insert (10);
    tree.insert (5);

    // std::cout << "root = " << tree.getRoot ()->parent_->val_ << std::endl;

    tree.graphDump ("out.dot");

    return 0;
}