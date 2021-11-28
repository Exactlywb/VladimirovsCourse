#include "matrix.hpp"
#include <fstream>
#include <gtest/gtest.h>

TEST( detTests, check2x2 ) {

    Math::Matrix<double> matr {2};
    EXPECT_DOUBLE_EQ (0, matr.calcDet ());

    matr.setElemUsingRowCol (0, 0, 1);
    matr.setElemUsingRowCol (0, 1, 15);
    matr.setElemUsingRowCol (1, 0, 24);
    matr.setElemUsingRowCol (1, 1, 67);

    EXPECT_DOUBLE_EQ (-293, matr.calcDet ());

    Math::Matrix<int> newMatr {2};
    EXPECT_EQ (0, newMatr.calcDet ());

    newMatr.setElemUsingRowCol (0, 0, 1);
    newMatr.setElemUsingRowCol (0, 1, 256);
    newMatr.setElemUsingRowCol (1, 0, 2);
    newMatr.setElemUsingRowCol (1, 1, 50);

    EXPECT_EQ (-462, newMatr.calcDet ());

}

TEST( detTests, check3x3 ) {

    Math::Matrix<double> matr {3};
    EXPECT_DOUBLE_EQ (0, matr.calcDet ());

    matr.setElemUsingRowCol (0, 0, 1);
    matr.setElemUsingRowCol (0, 1, 0);
    matr.setElemUsingRowCol (0, 2, 1);
    matr.setElemUsingRowCol (1, 0, 28);
    matr.setElemUsingRowCol (1, 1, 36);
    matr.setElemUsingRowCol (1, 2, 71);
    matr.setElemUsingRowCol (2, 0, 2);
    matr.setElemUsingRowCol (2, 1, 8);
    matr.setElemUsingRowCol (2, 2, 9);
    
    EXPECT_DOUBLE_EQ (-92, matr.calcDet ());

}

TEST( detTests, check4x4 ) {

    Math::Matrix<double> matr {4};
    EXPECT_DOUBLE_EQ (0, matr.calcDet ());

    matr.setElemUsingRowCol (0, 0, 3);
    matr.setElemUsingRowCol (0, 1, -3);
    matr.setElemUsingRowCol (0, 2, -5);
    matr.setElemUsingRowCol (0, 3, 8);
    matr.setElemUsingRowCol (1, 0, -3);
    matr.setElemUsingRowCol (1, 1, 2);
    matr.setElemUsingRowCol (1, 2, 4);
    matr.setElemUsingRowCol (1, 3, -6);
    matr.setElemUsingRowCol (2, 0, 2);
    matr.setElemUsingRowCol (2, 1, -5);
    matr.setElemUsingRowCol (2, 2, -7);
    matr.setElemUsingRowCol (2, 3, 5);
    matr.setElemUsingRowCol (3, 0, -4);
    matr.setElemUsingRowCol (3, 1, 3);
    matr.setElemUsingRowCol (3, 2, 5);
    matr.setElemUsingRowCol (3, 3, -6);
    EXPECT_DOUBLE_EQ (18, matr.calcDet ());

    matr.setElemUsingRowCol (0, 0, 0);
    matr.setElemUsingRowCol (0, 1, 0);
    matr.setElemUsingRowCol (0, 2, 0);
    matr.setElemUsingRowCol (0, 3, 0);
    EXPECT_DOUBLE_EQ (0, matr.calcDet ());
    
}

TEST( detTests, test1File ) {

    system ("./matrix < ../tests/test1 > test.out");
    
    std::ifstream resFile   ("test.out");
    std::string res         ((std::istreambuf_iterator<char>(resFile)),
                              std::istreambuf_iterator<char>());

    std::ifstream answFile  ("../tests/answ1");
    std::string answ        ((std::istreambuf_iterator<char>(answFile)),
                              std::istreambuf_iterator<char>());

    ASSERT_EQ(answ, res);

}

TEST( detTests, test2File ) {

    system ("./matrix < ../tests/test2 > test.out");
    
    std::ifstream resFile   ("test.out");
    std::string res         ((std::istreambuf_iterator<char>(resFile)),
                              std::istreambuf_iterator<char>());

    std::ifstream answFile  ("../tests/answ2");
    std::string answ        ((std::istreambuf_iterator<char>(answFile)),
                              std::istreambuf_iterator<char>());

    ASSERT_EQ(answ, res);

}

TEST( detTests, test3File ) {

    system ("./matrix < ../tests/test3 > test.out");
    
    std::ifstream resFile   ("test.out");
    std::string res         ((std::istreambuf_iterator<char>(resFile)),
                              std::istreambuf_iterator<char>());

    std::ifstream answFile  ("../tests/answ3");
    std::string answ        ((std::istreambuf_iterator<char>(answFile)),
                              std::istreambuf_iterator<char>());

    ASSERT_EQ(answ, res);

}

TEST( detTests, test4File ) {

    system ("./matrix < ../tests/test4 > test.out");
    
    std::ifstream resFile   ("test.out");
    std::string res         ((std::istreambuf_iterator<char>(resFile)),
                              std::istreambuf_iterator<char>());

    std::ifstream answFile  ("../tests/answ4");
    std::string answ        ((std::istreambuf_iterator<char>(answFile)),
                              std::istreambuf_iterator<char>());

    ASSERT_EQ(answ, res);

}

TEST( detTests, test5File ) {

    system ("./matrix < ../tests/test5 > test.out");
    
    std::ifstream resFile   ("test.out");
    std::string res         ((std::istreambuf_iterator<char>(resFile)),
                              std::istreambuf_iterator<char>());

    std::ifstream answFile  ("../tests/answ5");
    std::string answ        ((std::istreambuf_iterator<char>(answFile)),
                              std::istreambuf_iterator<char>());

    ASSERT_EQ(answ, res);

}

int main (int argc, char** argv) {

    testing::InitGoogleTest (&argc, argv);

    return RUN_ALL_TESTS ();

}
