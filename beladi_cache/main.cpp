#include "beladi.hpp"

int main () {

    size_t cacheSize        = 0;
    size_t cacheInputElems  = 0;

    std::cin >> cacheSize >> cacheInputElems;

    Beladi<int, int> cache (cacheSize, cacheInputElems);

    cache.readData ();

    size_t hits = cache.lookup ();

    std::cout << "Hits: " << hits << std::endl;

    return 0;

}
