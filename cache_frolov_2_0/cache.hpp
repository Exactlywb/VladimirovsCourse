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

    ListWrapper (size_t maxSizeVal = 0):
                maxSize (maxSizeVal) {}

    int overFlow () const {

        return mainList.size () > maxSize;

    }

};

template <typename elemType, typename keyType = int>
class Cache {

    size_t cacheSize;

    typedef typename std::list<std::pair<elemType, listType>>::iterator ListIterator;
    typedef typename std::unordered_map<keyType, ListIterator>::iterator HashIterator;
    std::unordered_map<keyType, ListIterator> hashTable;

    const double outSizeCoef    = 0.6;
    const double hotSizeCoef    = 0.2;
    const double inSizeCoef     = 0.2;

    ListWrapper<elemType> in    {(size_t)(cacheSize * inSizeCoef)};
    ListWrapper<elemType> out   {(size_t)(cacheSize * outSizeCoef)};
    ListWrapper<elemType> hot   {(size_t)(cacheSize * hotSizeCoef)};
    
    int insert_new_key (keyType key) {
    
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

    int handle_existed_key (HashIterator hashPage) {
    
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

public:
    Cache (size_t size):
        cacheSize (size)
    {}

    int lookup (keyType key) {

        auto hashPage = hashTable.find (key);
        
        if (hashPage == hashTable.end ())
            return insert_new_key (key);

        return handle_existed_key (hashPage);    
    
    }

};
