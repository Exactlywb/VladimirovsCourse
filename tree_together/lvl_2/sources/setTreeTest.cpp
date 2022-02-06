#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

int main ()
{
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

        answ.push_back (static_cast<int> (std::distance (tree.find(minBorder),tree.find(maxBorder))));
    }

    // for (auto v : answ)
    //     std::cout << v << std::endl;

    return 0;
}
