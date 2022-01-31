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

        if (maxBorder <= minBorder) {

            answ.push_back (0);
            continue;

        }

        auto minIt = tree.upper_bound (minBorder);
        auto maxIt = tree.lower_bound (maxBorder);

        answ.push_back (static_cast<int>(std::distance (minIt, maxIt)));

    }

    for (auto v: answ)
        std::cout << v << std::endl;

}
