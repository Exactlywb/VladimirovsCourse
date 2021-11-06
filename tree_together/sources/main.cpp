#include "tree.hpp"

int main () {

    TreeImpl::Tree mainTree {};

    int n{};
    std::cin >> n; 

    for (int i = 0; i <= n; ++i) {
        // int key;
        // std::cin >> key; 
        if (i == 14)
            continue;

        mainTree.push (i);
    }

    std::cout << "5-th = " << mainTree.KthOrderStatistic (5) << std::endl;
    std::cout << "Less than 500 " << mainTree.getNLessThan (500) << std::endl;

    mainTree.graphDump ("tree.dot");
    return 0;
}
  