#include "tree.hpp"
#include <vector>

int main () {

    TreeImpl::Tree mainTree {};
    std::vector<int> res {};

    char inputSymb   = 0;
    int numToHandle = 0;
    while (!std::cin.eof ()) {

        std::cin >> inputSymb;
        if (std::cin.eof ())
            break;
        
        std::cin >> numToHandle;

        if (inputSymb == 'k')
            mainTree.push (numToHandle);
        else if (inputSymb == 'm')
            res.push_back (mainTree.KthOrderStatistic (numToHandle));
        else if (inputSymb == 'n')
            res.push_back (mainTree.getNLessThan (numToHandle));
        else {

            std::cout << "Wrong command " << inputSymb << std::endl;
            break;
            
        }

    }

    for (auto val:res)
        std::cout << val << " ";
    std::cout << std::endl;

    return 0;
}
  
 
