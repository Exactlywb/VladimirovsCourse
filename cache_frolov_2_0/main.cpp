#include "cache.hpp"

int main () {

    size_t cacheSize        = 0;
    size_t cacheInputElems  = 0;

    std::cin >> cacheSize >> cacheInputElems;

    Cache <int, int> mainCache (cacheSize);

    size_t hits = 0;
    for (size_t curPage = 0; curPage < cacheInputElems; curPage++) {

        int key = 0;
        std::cin >> key;

        hits += mainCache.lookup (key);

    }

    std::cout << hits << std::endl;

    return 0;

}
