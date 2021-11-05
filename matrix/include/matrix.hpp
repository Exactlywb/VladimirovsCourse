#ifndef MATRIX_HPP__
#define MATRIX_HPP__

#include <iostream>
#include <cstring>

namespace Math {

    template <typename T = double>
    class Matrix {

        T*  data;

        int rowNum;
        int colNum;

    public:
        //Constructor
        Matrix (int rN, int cN): 
            rowNum (rN),
            colNum (cN) {
            
            data = new T [rowNum + colNum];

        }

        Matrix (int n = 0):
            rowNum (n),
            colNum (n) {

            data = new T [rowNum + colNum];

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

    };

};

#endif
