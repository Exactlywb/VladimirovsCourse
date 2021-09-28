#include <iostream>
#include <unordered_map>

template <typename Elem, typename keyType = int>
class Beladi {

    size_t cacheSize;

    Elem* forFuture;
    std::unordered_map<keyType, Elem> hashTable;

    size_t cacheInputElems;

public:
    Beladi (size_t size, size_t inputN_ = 0):
            cacheSize (size),
            cacheInputElems (inputN_){

        forFuture = new Elem [cacheInputElems];

    }

    ~Beladi () {

        delete [] forFuture;

    }

    size_t lookup () {

        size_t hits = 0;
        for (size_t curPage = 0; curPage < cacheInputElems; curPage++) {

            auto findInCache = hashTable.find (forFuture [curPage]);
            if (findInCache == hashTable.end ()) {

                if (hashTable.size () < cacheSize) {

                    hashTable.insert ({forFuture [curPage], forFuture [curPage]});
                    continue;

                }

                int commonLastIn = -1;
                for (auto cacheElem : hashTable) {
                    
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
                        hashTable.erase (hashTable.find(cacheElem.second));
                        break;

                    }

                    if (lastIn > commonLastIn)
                        commonLastIn = lastIn;

                }

                if (commonLastIn != -1)
                    hashTable.erase (hashTable.find (forFuture [commonLastIn]));

                hashTable.insert ({forFuture [curPage], forFuture [curPage]});

            } else
                hits++;

        }

        return hits;

    }

    void readData () {

        for (size_t curPage = 0; curPage < cacheInputElems; curPage++) {

            int key = 0;
            std::cin >> key;

            forFuture [curPage] = key;

        }

    }

};
