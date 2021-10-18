#include <iostream>
#include "include/octotree.hpp"

const int numTests = 7;


int TestIntersectionFunction () {

    // const char* nameTests[numTests] = {"../tests/test7.txt"};

    const char* nameTests[numTests] = {"../tests/trXseg/test1.txt", "../tests/trXseg/test2.txt", "../tests/trXseg/test3.txt", "../tests/trXseg/test4.txt", "../tests/trXseg/test5.txt",
                                       "../tests/trXseg/test6.txt", "../tests/trXseg/test7.txt"};


    for (int i = 0; i < numTests; ++i) {

        FILE* ftest;

        

        ftest = freopen (nameTests[i], "r", stdin);
            if (ftest == NULL) {

                perror ("File open error:");
                return -1;
            }

        GObjects::Triangle tr1 {};
        GObjects::Triangle tr2 {};
        std::cin >> tr1 >> tr2;
        assert (std::cin.good());

        bool result = GObjects::Intersect3DTriangles(tr1, tr2);

        bool trueResult;
        std::cin >> trueResult;

        if (result == trueResult)
            std::cout << "TEST " << i + 1 << " PASSED!" << std::endl;
        else    
            std::cout << "TEST " << i + 1 << " WRONG!" << std::endl;

        fclose (ftest);
    }

    return 0;   

}

int main()
{
// int countTriangles;
// std::cin >> countTriangles;

// Tree::Octotree tree;

// tree.fillTree(countTriangles);

    // GObjects::Triangle tr1 {};
    // GObjects::Triangle tr2 {};
    // std::cin >> tr1 >> tr2;
    // assert (std::cin.good());

    // std::cout << tr1;
    // std::cout << tr2;
    
    // bool test = GObjects::Intersect3DTriangles (tr1, tr2);
    // if (test)
    //     std::cout << "Intersect" << std::endl;
    // else
    //     std::cout << "Not :(" << std::endl;

    if ( (TestIntersectionFunction ()) == -1)
        std::cout << "Troubles with files!" << std::endl;

    return 0;
}