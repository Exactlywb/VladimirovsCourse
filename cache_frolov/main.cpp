#include <iostream>
#include <iterator>

#include <list>
#include <unordered_map>

class Cache {

    typedef typename std::list<int>::iterator ListIterator;

    size_t inSize;
    size_t outSize;
    size_t hotSize;

    size_t curInSize;
    size_t curOutSize;
    size_t curHotSize;

//FIELDS
    std::list<int> commonList;

    std::unordered_map<int, ListIterator> hashTable;

public:
    Cache   (size_t cacheSize);
    ~Cache  ();

    size_t  getCurInSize    ();
    size_t  getCurOutSize   ();
    size_t  getCurHotSize   ();

    size_t  getInSize       ();
    size_t  getOutSize      ();
    size_t  getHotSize      ();

    std::list<int>* getList ();
    std::unordered_map<int, ListIterator>* getTable () {

        return &hashTable;

    }

    size_t  getPage     (int pageVal);

};

int main () {

    size_t cacheSize = 0;
    size_t elemCount = 0;

    std::cin >> cacheSize >> elemCount;

    size_t  hits = 0;
    int     page = 0;

    Cache mainCache (cacheSize);
    for (size_t curPageNum = 0; curPageNum < elemCount; curPageNum++) {
        
        std::cin >> page;
        hits += mainCache.getPage (page);
        
    }

    std::cout << "Hits = " << hits << std::endl;

    std::cout << std::endl;

    return 0;

}

Cache::Cache (size_t cacheSize) {

    hotSize = cacheSize / 5;
    outSize = 3 * (cacheSize / 5);
    inSize  = cacheSize - outSize - hotSize;

    curInSize   = 0;
    curOutSize  = 0;
    curHotSize  = 0;

}

Cache::~Cache () {

    inSize  = 0;
    outSize = 0;

    hotSize = 0;

}

size_t Cache::getHotSize () {

    return hotSize;

}

size_t Cache::getInSize () {

    return inSize;

}

size_t Cache::getOutSize () {

    return outSize;

}

size_t Cache::getCurInSize () {

    return curInSize;

}

size_t Cache::getCurOutSize () {

    return curOutSize;

}

size_t Cache::getCurHotSize () {

    return curHotSize;

}

size_t Cache::getPage (int pageVal) {

    auto pageIt = hashTable.find (pageVal);

    auto listBegin = commonList.begin ();
    if (pageIt == hashTable.end ()) {   //miss
        
        commonList.push_front (pageVal);
        listBegin = commonList.begin ();
        hashTable.insert ({pageVal, listBegin });

        if (curInSize == inSize) {

            if (curOutSize == outSize) {
                
                std::advance (listBegin, outSize + inSize);

                auto oldElemMapIt = hashTable.find (*listBegin);
                hashTable.erase (oldElemMapIt);
                commonList.erase (listBegin);

            } else
                curOutSize++;

        } else
            curInSize++;

        return 0;

    }

    auto curListNode = (*pageIt).second;

    int dist = std::distance (listBegin, curListNode);

    if (dist >= curInSize) { //elem exists in out or hot buffer

        if (dist >= curInSize + curOutSize) { //elem exists in hot buffer

            std::advance (listBegin, curInSize + curOutSize); // Think
            commonList.splice (listBegin, commonList, curListNode);

        } else { //elem exists in out buffer

            std::advance (listBegin, curInSize + curOutSize);
 
            commonList.splice (listBegin, commonList, curListNode);

            if (curHotSize == hotSize) {
                
                auto lastElem = commonList.back ();
                auto lastElemMapIt = hashTable.find (lastElem);

                hashTable.erase (lastElemMapIt);
                commonList.pop_back ();

            }
            else 
                curHotSize++;

            curOutSize--;

        }

    }

    return 1;

}

std::list<int>* Cache::getList () {

    return &commonList;

}
