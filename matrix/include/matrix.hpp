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

        T calc2x2MatrixDet () const {

            T mainDiagonal = data [0] * data [3];
            T sideDiagonal = data [1] * data [2];

            return mainDiagonal - sideDiagonal;

        }

        T calcDetUsingTrMeth () const {    //For 3x3 matrix only
            
            T mainDiagonal  = data [0] * data [4] * data [8];

            T posStraightTr = data [1] * data [5] * data [6];
            T posSideTr     = data [2] * data [3] * data [7];

            T sideDiagonal  = data [2] * data [4] * data [6];

            T negStraightTr = data [0] * data [5] * data [7];
            T negSideTr     = data [1] * data [3] * data [8];

            return  mainDiagonal + posStraightTr + posSideTr - 
                    sideDiagonal - negStraightTr - negSideTr;

        }

        T calcUsingGaussMeth () const {
            
            static const eps = 1e-10;
            
            T det = 1;
            for (int i = 0; i < rowNum; ++i) {

                int curRowNum = i;
                for (int nextRowNum = i + 1; nextRowNum < rowNum; ++nextRowNum) {

                    if (std::abs (getElemUsingRowCol (nextRowNum, i)) > 
                        std::abs (getElemUsingRowCol (curRowNum, i)))
                        curRowNum = nextRowNum;

                }

                if (!DblCmp (getElemUsingRowCol (curRowNum, i), 0))
                    return T {};

                std::swap (...); 

                if (i != curRowNum)
                    det = -det;
                
                det *= getElemUsingRowCol (i, i);
                for (int curColNum = i + 1; curColNum < rowNum; ++curColNum) {

                    //!TODO
                    
                }

            }

            return det;

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

        //!TODO test constructors

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

            toMove.data = nullptr;  //stolen -_-

        }

        //Move assignment
        Matrix& operator= (Matrix<T> &&toMove) noexcept {

            if (this == &toMove)    //bad-bad things...
                return *this;
            
            delete[] data;
            data = toMove.data;         
            toMove.data = nullptr;  //stolen -_-

            colNum = toMove.colNum;
            rowNum = toMove.rowNum;

            return *this;

        }

        //Destructor
        ~Matrix () {
            delete[] data;
        }

        T& operator[] (const int ind) {
            return data [ind];
        }

        const T& operator[] (const int ind) const {
            return data [ind];
        }

        //PseudoOperators
        T getElemUsingRowCol (const int col, const int row) const {

            // The motivation for creating this function is that
            //I don't want to create proxy structures, cause it looks
            //more cache friendly when we use T* data as a straight
            //array. Maybe I'm wrong.

            if (CheckCorrectRowRequest (row) != 0) return T {};
            if (CheckCorrectColRequest (col) != 0) return T {};

            return data [row * colNum + col];

        }
        
        void setElemUsingRowNumAndColNum (const int col, const int row, T& elem) {

            // Similar with GetElemUsingRowNumAndColNum

            if (CheckCorrectRowRequest (row) != 0) return;
            if (CheckCorrectColRequest (col) != 0) return;

            data [row * colNum + col] = elem;
            
        }

        void setElemUsingRowNumAndColNum (const int col, const int row, T elem) {

            // Similar with GetElemUsingRowNumAndColNum

            if (CheckCorrectRowRequest (row) != 0) return;
            if (CheckCorrectColRequest (col) != 0) return;

            data [row * colNum + col] = elem;
            
        }

        void matrixDump () const {

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

        T calcDet () const {
                
            if (rowNum != colNum) {

                std::cout << "I dunno how to calculate non-square matrix's determinate :(" << std::endl;
                return -666;

            }

            if (rowNum == 1)                    // Little optimization cause
                return data [0];                //it looks irrational to call
            else if (rowNum == 2)               //Gauss method on matrix less
                return calc2x2MatrixDet ();     //3x3 size.
            else if (rowNum == 3)
                return calcDetUsingTrMeth ();   // Triangle method for 3x3 matrix.
            else
                return calcUsingGaussMeth ();   //!TODO think about triangle matrix
            
        }

    };

}

#endif
