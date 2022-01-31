#include "tree.hpp"
#include <vector>

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

        auto minIt = tree.upperBound (minBorder);
        auto maxIt = tree.lowerBound (maxBorder);
        
    #if 0   
        std::cout << "minBorder: " << minBorder << std::endl;
        std::cout << "maxBorder: " << maxBorder << std::endl;

        std::cout << "minIt->val " << minIt->val_ << std::endl;
        std::cout << "maxIt->val " << maxIt->val_ << std::endl;

        std::cout << "distance:  " << std::distance (minIt, maxIt) << std::endl;
    #endif 

        answ.push_back (static_cast<int>(std::distance (minIt, maxIt)));

    }

    for (auto v: answ)
        std::cout << v << std::endl;

    return 0;
}