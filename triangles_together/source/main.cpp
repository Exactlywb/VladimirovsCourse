#include <iostream>
#include "octree.hpp"

//=====================================================================================================

int GetTriangles ();
int TestIntersectionFunction ();

//=====================================================================================================

int main()
{
    
#ifdef DEBUG
    if (TestIntersectionFunction ())
        std::cout << "Problems with file" << std::endl;
#else
    GetTriangles ();
#endif
    return 0;
}

//=====================================================================================================

int TestIntersectionFunction () {

    const int numTests = 26;
    const char* nameTests[numTests] = {"../tests/trXtr/test1.txt", "../tests/trXtr/test2.txt", "../tests/trXtr/test3.txt", "../tests/trXtr/test4.txt", 
                                       "../tests/trXtr/test5.txt", "../tests/trXtr/test6.txt", "../tests/trXtr/test7.txt", "../tests/trXtr/test8.txt", 
                                       "../tests/trXseg/test1.txt", "../tests/trXseg/test2.txt", "../tests/trXseg/test3.txt", "../tests/trXseg/test4.txt", 
                                       "../tests/trXseg/test5.txt", "../tests/trXseg/test6.txt", "../tests/trXseg/test7.txt", "../tests/trXseg/test8.txt",
                                       "../tests/trXseg/test9.txt", "../tests/trXseg/test10.txt", "../tests/segXseg/test1.txt", "../tests/segXseg/test2.txt", 
                                       "../tests/segXseg/test3.txt", "../tests/segXseg/test4.txt", "../tests/segXseg/test5.txt", "../tests/segXseg/test6.txt", 
                                       "../tests/segXseg/test7.txt", "../tests/segXseg/test8.txt"};

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

