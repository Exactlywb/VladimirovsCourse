#include "tree.hpp"

int main ()
{
    TreeImpl::SplayTree<int> tree;
    
    

    for (int i = 0; i < 100; ++i) {
        // int x;
        // std::cin >> x;
        tree.insert (i);
    }

    auto first = tree.begin();
    auto end = tree.end();

    std::cout << *(tree.find(123)) << std::endl;

    for (auto v : tree) {
        std::cout << v->val_ << " ";
    }

    tree.graphDump ("out.dot");

    return 0;
}