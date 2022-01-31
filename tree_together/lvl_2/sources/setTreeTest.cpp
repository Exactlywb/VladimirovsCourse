#include <iostream>
#include <set>
#include <vector>
#include <algorithm>

int main () {

    std::set<int> tree;

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

        auto minItInserted = tree.insert (minBorder);
        auto maxItInserted = tree.insert (maxBorder);

        auto minIt = tree.upper_bound (minBorder);
        auto maxIt = tree.lower_bound (maxBorder);

        if (minItInserted.first == maxItInserted.first) {

            std::cout << "equal borders!" << std::endl;
            answ.push_back (0);
            continue;

        }

        answ.push_back (static_cast<int>(std::distance (minIt, maxIt)));

        if (minItInserted.second)
            tree.erase (minBorder);
        if (maxItInserted.second)
            tree.erase (maxBorder);

    }

    for (auto v: answ)
        std::cout << v << std::endl;

}
