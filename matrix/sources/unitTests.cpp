#include "matrix.hpp"
#include <gtest/gtest.h>
#include <fstream>

const double EPS = 1e-5;

void RunDetTest (const char* testSrc, const char* answSrc) {

    std::ifstream test (testSrc);
    int size = 0;
    test >> size;

    Matrix::Matrix<double> matr (size, size);

    for (int y = 0; y < size; ++y) {

        for (int x = 0; x < size; ++x)
            test >> matr [y] [x];

    }

    test.close ();

    std::ifstream answ (answSrc);

    double answVal = 0;
    answ >> answVal;

    answ.close ();
    double evaluatedDet = matr.det ();

    ASSERT_EQ (std::abs (evaluatedDet - answVal) < EPS, true);

}

TEST (detTest, test8) {
    RunDetTest ("../tests/test8.dat", "../tests/answ8.dat");
}

TEST (detTest, test7) {
    RunDetTest ("../tests/test7.dat", "../tests/answ7.dat");
}

TEST (detTest, test6) {
    RunDetTest ("../tests/test6.dat", "../tests/answ6.dat");
}

TEST (detTest, test5) {
    RunDetTest ("../tests/test5.dat", "../tests/answ5.dat");
}

TEST (detTest, test4) {
    RunDetTest ("../tests/test4.dat", "../tests/answ4.dat");
}

TEST (detTest, test3) {
    RunDetTest ("../tests/test3.dat", "../tests/answ3.dat");
}

TEST (detTest, test2) {
    RunDetTest ("../tests/test2.dat", "../tests/answ2.dat");
}

TEST (detTest, test1) {
    RunDetTest ("../tests/test1.dat", "../tests/answ1.dat");
}

TEST (copyTestCtr, test1) {

    std::ifstream test ("../tests/test5.dat");
    int size = 0;
    test >> size;

    Matrix::Matrix<double> matr (size, size);

    for (int y = 0; y < size; ++y) {

        for (int x = 0; x < size; ++x)
            test >> matr [y] [x];

    }

    test.close ();

    Matrix::Matrix<double> copyMatr (matr);

    ASSERT_EQ (matr, copyMatr);

}

TEST (copyTestCtr, test2) {

    std::ifstream test ("../tests/test8.dat");
    int size = 0;
    test >> size;

    Matrix::Matrix<int> matr (size, size);

    for (int y = 0; y < size; ++y) {

        for (int x = 0; x < size; ++x)
            test >> matr [y] [x];

    }

    test.close ();

    Matrix::Matrix<int> copyMatr (matr);

    ASSERT_EQ (matr, copyMatr);

}

TEST (moveTestCtr, test1) {

    std::ifstream test ("../tests/test7.dat");
    int size = 0;
    test >> size;

    Matrix::Matrix<int> matr (size, size);

    for (int y = 0; y < size; ++y) {

        for (int x = 0; x < size; ++x)
            test >> matr [y] [x];

    }

    test.close ();

    Matrix::Matrix<int> copyMatr (matr);
    Matrix::Matrix<int> moveMatr (std::move (copyMatr));

    ASSERT_EQ (moveMatr, matr);

}

TEST (moveTestCtr, test2) {

    std::ifstream test ("../tests/test8.dat");
    int size = 0;
    test >> size;

    Matrix::Matrix<double> matr (size, size);

    for (int y = 0; y < size; ++y) {

        for (int x = 0; x < size; ++x)
            test >> matr [y] [x];

    }

    test.close ();

    Matrix::Matrix<double> copyMatr (matr);
    Matrix::Matrix<double> moveMatr (std::move (copyMatr));

    ASSERT_EQ (moveMatr, matr);

}

TEST (copyTestAssign, test1) {

    std::ifstream test ("../tests/test8.dat");
    int size = 0;
    test >> size;

    Matrix::Matrix<int> matr (size, size);

    for (int y = 0; y < size; ++y) {

        for (int x = 0; x < size; ++x)
            test >> matr [y] [x];

    }

    test.close ();

    Matrix::Matrix<int> copyMatr (1, 1);
    copyMatr = matr;

    ASSERT_EQ (copyMatr, matr);

}

TEST (copyTestAssign, test2) {

    std::ifstream test ("../tests/test5.dat");
    int size = 0;
    test >> size;

    Matrix::Matrix<double> matr (size, size);

    for (int y = 0; y < size; ++y) {

        for (int x = 0; x < size; ++x)
            test >> matr [y] [x];

    }

    test.close ();

    Matrix::Matrix<double> copyMatr (1, 1);
    copyMatr = matr;

    ASSERT_EQ (copyMatr, matr);

}

TEST (moveTestAssign, test1) {

    std::ifstream test ("../tests/test8.dat");
    int size = 0;
    test >> size;

    Matrix::Matrix<double> matr (size, size);

    for (int y = 0; y < size; ++y) {

        for (int x = 0; x < size; ++x)
            test >> matr [y] [x];

    }

    test.close ();

    Matrix::Matrix<double> copyMatr (matr);
    Matrix::Matrix<double> moveMatr (1, 1);
    moveMatr = std::move (copyMatr);

    ASSERT_EQ (moveMatr, matr);

}

TEST (moveTestAssign, test2) {

    std::ifstream test ("../tests/test8.dat");
    int size = 0;
    test >> size;

    Matrix::Matrix<int> matr (size, size);

    for (int y = 0; y < size; ++y) {

        for (int x = 0; x < size; ++x)
            test >> matr [y] [x];

    }

    test.close ();

    Matrix::Matrix<int> copyMatr (matr);
    Matrix::Matrix<int> moveMatr (1, 1);
    moveMatr = std::move (copyMatr);

    ASSERT_EQ (moveMatr, matr);

}

int main (int argc, char **argv) {

    testing::InitGoogleTest (&argc, argv);
    return RUN_ALL_TESTS ();

}
