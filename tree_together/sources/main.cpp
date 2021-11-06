#include "tree.hpp"

int main () {

    TreeImpl::Tree mainTree {};

    int n{};
    std::cin >> n; 

    for (int i = 0; i < n; ++i) {
        // int key;
        // std::cin >> key; 

        mainTree.push (i);
    }

    mainTree.graphDump ("tree.dot");
    return 0;
}
  