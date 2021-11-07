#ifndef MATRIX_HPP__
#define MATRIX_HPP__

#include <iostream>
#include <cstring>
#include <iomanip>

namespace Math {

    template <typename T = double>
    class Matrix {

        T*  data;

        int rowNum;
        int colNum;

    private:
        int CheckCorrectRowRequest (const int row) {

            if (row >= rowNum) {

                std::cout   << "Max row number is "  << rowNum - 1
                            << " but you requested " << row << std::endl;
                return -1;

            }

            return 0;
            
        }

        int CheckCorrectColRequest (const int col) {

            if (col >= colNum) {

                std::cout   << "Max col number is "  << colNum - 1
                            << " but you requested " << col << std::endl;
                return -1;

            }

            return 0;
            
        }

    public:
        //Constructor
        Matrix (int rN, int cN): 
            rowNum (rN),
            colNum (cN) {
            
            data = new T [rowNum * colNum];

        }

        Matrix (int n = 0):
            rowNum (n),
            colNum (n) {

            data = new T [n * n];

        }

        //Copy constructor
        Matrix (const Matrix &toCopy):
            rowNum      (toCopy.rowNum),
            colNum      (toCopy.colNum) {

            data = new T [rowNum + colNum];
            memcpy (data, toCopy.data, rowNum + colNum);

        }

        //Copy assignments
        Matrix& operator= (const Matrix& toCopy) {

            if (this == &toCopy)    //scary and cold
                return *this;

            rowNum = toCopy.rowNum;
            colNum = toCopy.colNum;

            int fullSize    = rowNum + colNum;
            T* dataTmp      = new T [fullSize];
            memcpy (dataTmp, toCopy.data, fullSize);
            delete[] data;
            data = dataTmp;

            return *this;

        }

        //Move constructor
        Matrix (Matrix<T> &&toMove) noexcept :
            data (toMove.data),
            colNum (toMove.colNum),
            rowNum (toMove.rowNum) {

            toMove.data = nullptr; //stolen -_-

        }

        //Move assignment
        Matrix& operator= (Matrix<T> &&toMove) noexcept {

            if (this == &toMove)     //bad-bad things...
                return *this;
            
            delete[] data;
            data = toMove.data;
            toMove.data = nullptr;   //stolen -_-

            colNum = toMove.colNum;
            rowNum = toMove.rowNum;

            return *this;

        }

        //Destructor
        ~Matrix () {
            delete[] data;
        }

        //Operators
        T operator[] (const int ind) {
            return data [ind];
        }

        //PseudoOperators
        T GetElemUsingRowNumAndColNum (const int col, const int row) const {

            // The motivation for creating this function is that
            //I don't want to create proxy structures, cause it looks
            //more cache friendly when we use T* data as a straight
            //array. Maybe I'm wrong.

            if (CheckCorrectRowRequest (row) != 0) return T {};
            if (CheckCorrectColRequest (col) != 0) return T {};

            return data [row * col + col];

        }
        
        void SetElemUsingRowNumAndColNum (const int col, const int row, T& elem) {

            // Similar with GetElemUsingRowNumAndColNum

            if (CheckCorrectRowRequest (row) != 0) return;
            if (CheckCorrectColRequest (col) != 0) return;

            data [row * col + col] = elem;
            
        }

        void MatrixDump () const {

            std::cout << std::endl;
            for (int curRow = 0; curRow < rowNum; curRow++) {
                
                int curBegInd = curRow * colNum;
                std::cout << "[";
                for (int curCol = 0; curCol < colNum; curCol++) {

                    std::cout << std::setw (5) << data [curBegInd + curCol];
                    if (curCol + 1 < colNum)
                        std::cout << " ";
            
                }
                std::cout << "]" << std::endl;

            }

        }

    };

}

#endif
