#include "tree.hpp"

int main ()
{
    TreeImpl::SplayTree<int> tree;

    for (int i = 0; i < 100000; ++i) {
        int x;
        std::cin >> x;
        tree.insert (x);
    }
    std::cout << tree.begin ()->val_ << std::endl;

    tree.graphDump ("out.dot");

    return 0;
}