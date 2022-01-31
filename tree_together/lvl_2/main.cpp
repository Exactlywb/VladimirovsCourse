#include "include/tree.hpp"

int main()
{
    TreeImpl::SplayTree<int> tree;
    tree.graphDump ("out.dot");

    return 0;
}