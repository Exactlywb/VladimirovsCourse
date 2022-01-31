#include "include/tree.hpp"

int main()
{
    TreeImpl::SplayTree<int> tree;

    for (int i = 0; i < 30; ++i) {
        int x;
        std::cin >> x;
        tree.insert (x);
    }

    tree.graphDump ("out.dot");

    return 0;
}