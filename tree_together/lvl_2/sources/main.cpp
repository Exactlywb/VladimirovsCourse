#include "tree.hpp"

int main ()
{
    TreeImpl::SplayTree<int> tree;
    
    

    for (int i = 0; i < 100; ++i) {
        // int x;
        // std::cin >> x;
        tree.insert (i);
    }
    std::cout << tree.begin ()->val_ << std::endl;

    std::cout << *(tree.find(123)) << std::endl;



    tree.graphDump ("out.dot");

    return 0;
}