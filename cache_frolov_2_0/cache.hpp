#include <iostream>

#include <list>
#include <unordered_map>

enum listType {

    IN,
    OUT,
    HOT

};

template<typename elemType>
struct ListWrapper {

    size_t maxSize;
    
    std::list<std::pair<elemType, listType>> mainList;

    ListWrapper (size_t maxSizeVal):
                maxSize (maxSizeVal) {}
    
    ListWrapper ():
                maxSize (0) {}

    int overFlow () const {

        return mainList.size () > maxSize;

    }

    ~ListWrapper () {

        maxSize = 0;

    }

};

template <typename elemType, typename keyType = int>
struct Cache {

    size_t cacheSize;

    typedef typename std::list<std::pair<elemType, listType>>::iterator ListIterator;
    std::unordered_map<keyType, ListIterator> hashTable;

    ListWrapper<elemType> in;
    ListWrapper<elemType> out;
    ListWrapper<elemType> hot;

    Cache (size_t size):
            in (size - 4 * (size / 5)),
            out (3 * (size / 5)),
            hot (size / 5)
    {}

    ~Cache <elemType, keyType> () {

        cacheSize = 0;

    }

    int lookup (keyType key) {

        auto hashPage = hashTable.find (key);
        
        if (hashPage == hashTable.end ()) {

            in.mainList.push_front ({key, IN});
            hashTable.insert ({key, in.mainList.begin ()});

            if (in.overFlow ()) {

                auto inLastElem = (hashTable.find (in.mainList.back ().first))->second;
                inLastElem->second = OUT;

                auto outFirstElem = out.mainList.begin ();
                out.mainList.splice (outFirstElem, in.mainList, inLastElem);

                hashTable.erase (inLastElem->first);

                hashTable.insert ({inLastElem->first, out.mainList.begin ()});
                
                if (out.overFlow ()) {

                    auto outLastElem        = out.mainList.back ();

                    auto outLastElemHash    = hashTable.find (outLastElem.first);

                    hashTable.erase (outLastElemHash);
                    
                    out.mainList.pop_back ();

                }

            }

            return 0;

        }

        auto elemIt     = hashPage->second;
        auto listType   = elemIt->second;

        if (listType == OUT) {

            elemIt->second = HOT;
            auto hotBegin = hot.mainList.begin ();
            hot.mainList.splice (hotBegin, out.mainList, elemIt);

            if (hot.overFlow ()) {

                auto lastHotElemKey     = (hot.mainList.back ()).first;
                auto lastHotElemHash    = hashTable.find (lastHotElemKey);

                hashTable.erase (lastHotElemHash);
                hot.mainList.pop_back ();

            }

            hashTable.erase (hashPage);

            auto hotBeginIt = hot.mainList.begin ();
            hashTable.insert ({hotBeginIt->first, hotBeginIt});

        } else if (listType == HOT)
            hot.mainList.splice (hot.mainList.begin (), hot.mainList, elemIt);

        return 1;

    }

};
