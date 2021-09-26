#include "cache.hpp"

static void RunSingleTest (const std::pair<std::string, size_t> &test) {

    if (freopen (test.first.c_str (), "r", stdin) == nullptr) {

        std::cout << test.first << " <- this file is an asshole. Can't find it." << std::endl;
        return;

    }

    size_t cacheSize        = 0;
    size_t cacheInputElems  = 0;

    std::cin >> cacheSize >> cacheInputElems;
    if (!std::cin.good ())
        std::cout << "********** BAD FILE READING : " << test.first << std::endl;

    Cache <int, int> mainCache (cacheSize);

    size_t hits = 0;

    for (size_t curPage = 0; curPage < cacheInputElems; curPage++) {

        int key = 0;
        std::cin >> key;

        hits += mainCache.lookup (key);

    }

    fclose  (stdin);

    if (hits != test.second) {

        std::cout << "[" << test.first << "]" << " ! this test is an asshole!" << std::endl;
        std::cout << " * Expetected : " << test.second << std::endl;
        std::cout << " * Got : " << hits << std::endl;

    } else {

        std::cout << "[" << test.first << "]" << " ! test did it!" << std::endl;

    }

    std::cin.clear ();

}

int main () {

    const std::pair<std::string, size_t> tests [] = {
                            {"tests/test1.txt", 183},
                            {"tests/test2.txt", 459},
                            {"tests/test3.txt", 23},
                            {"tests/test4.txt", 369},
                            {"tests/test5.txt", 2285},
                            {"tests/test6.txt", 10},
                            {"tests/test7.txt", 32},
                            {"tests/test8.txt", 7},
                            {"tests/test9.txt", 10},       // I am not and idiot, but 10th test is a monster. I'm for real.
                            {"tests/test11.txt", 1998071}
    };

    for (size_t i = 0; i < 10; i++)
        RunSingleTest (tests [i]);

    return 0;

}

