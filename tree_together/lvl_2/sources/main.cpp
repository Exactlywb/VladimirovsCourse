#include <vector>

#include "tree.hpp"

int main ()
{
    TreeImpl::SplayTree<int> tree;

    int nElems = 0;
    std::cin >> nElems;

    int num = 0;
    for (int i = 0; i < nElems; ++i) {
        std::cin >> num;
        tree.insert (num);
    }

    // tree.graphDump("out.dot");

    int requests = 0;
    std::cin >> requests;

    std::vector<int> answ;

    int minBorder = 0;
    int maxBorder = 0;
    for (int i = 0; i < requests; ++i) {
        std::cin >> minBorder >> maxBorder;

        if (maxBorder <= minBorder) {
            answ.push_back (0);
            continue;
        }

#if 0
        auto minIt = tree.upperBound (minBorder);
        auto maxIt = tree.lowerBound (maxBorder);
        int length = static_cast<int>(std::distance (minIt, maxIt));
        int myL = tree.range (minBorder, maxBorder);
        
        answ.push_back (static_cast<int>(std::distance (minIt, maxIt)));
#endif

        answ.push_back (tree.range (minBorder, maxBorder));
    }

    // for (auto v : answ)
    //     std::cout << v << std::endl;

    return 0;
}