#include "tree.hpp"

int main ()
{
    TreeImpl::SplayTree<int> tree;

    for (int i = 0; i < 100000; ++i) {
        int x;
        std::cin >> x;
        tree.insert (x);
    }

    using it = TreeImpl::SplayTree<int>::MyIterator;

    auto first = tree.begin();
    auto end = tree.end();

    tree.graphDump ("out.dot");

    return 0;
}