#include <chrono>
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

    int requests = 0;
    std::cin >> requests;

    std::vector<int> answ;

    int minBorder = 0;
    int maxBorder = 0;

    auto start = std::chrono::steady_clock::now ();

    for (int i = 0; i < requests; ++i) {
        std::cin >> minBorder >> maxBorder;

        if (maxBorder <= minBorder) {
            answ.push_back (0);
            continue;
        }

        answ.push_back (tree.range (minBorder, maxBorder));
    }
    auto end = std::chrono::steady_clock::now ();

    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "elapsed time: " << elapsed_seconds.count () << "s\n";

#if 0
    for (auto v : answ)
        std::cout << v << std::endl;
#endif

    return 0;
}