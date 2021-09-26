#include <iostream>
#include <unordered_map>

static  void    ReadCache       (int* forFuture, const size_t cacheInputElems);
static  size_t  BeladiLookup    (int* forFuture, std::unordered_map<int, int>& cache, 
                                const size_t cacheInputElems, const size_t maxCacheSize);

int main () {

    size_t cacheSize        = 0;
    size_t cacheInputElems  = 0;

    std::cin >> cacheSize >> cacheInputElems;

    std::unordered_map<int, int> cache;
    int* forFuture = new int [cacheInputElems];

    ReadCache (forFuture, cacheInputElems);

    std::unordered_map<int, int>& cacheRef = cache;
    size_t hits = BeladiLookup (forFuture, cacheRef, cacheInputElems, cacheSize);

    std::cout << "Hits: " << hits << std::endl;

    return 0;

}

static void ReadCache (int* forFuture, const size_t cacheInputElems) {

    for (size_t curPage = 0; curPage < cacheInputElems; curPage++) {

        int key = 0;
        std::cin >> key;

        forFuture [curPage] = key;

    }

}

static size_t BeladiLookup (int* forFuture, std::unordered_map<int, int>& cache, 
                            const size_t cacheInputElems, const size_t maxCacheSize) {

    size_t hits = 0;
    for (size_t curPage = 0; curPage < cacheInputElems; curPage++) {

        auto findInCache = cache.find (forFuture [curPage]);
        if (findInCache == cache.end ()) {

            if (cache.size () < maxCacheSize) {

                cache.insert ({forFuture [curPage], forFuture [curPage]});
                continue;

            }

            int commonLastIn = -1;
            for (auto cacheElem : cache) {
                
                int lastIn = -1;
                int value = cacheElem.second;
                for (size_t futureInd = curPage; futureInd < cacheInputElems; futureInd++) {

                    if (forFuture [futureInd] == value) {

                        lastIn = futureInd;
                        break;

                    }
                        
                }

                if (lastIn == -1) {

                    commonLastIn = -1;
                    cache.erase (cache.find(cacheElem.second));
                    break;

                }

                if (lastIn > commonLastIn)
                    commonLastIn = lastIn;

            }

            if (commonLastIn != -1)
                cache.erase (cache.find (forFuture [commonLastIn]));

            cache.insert ({forFuture [curPage], forFuture [curPage]});

            continue;

        }

        hits++;

    }

    return hits;

}
