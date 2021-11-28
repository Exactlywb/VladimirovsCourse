#ifndef MATRIX_HPP__
#define MATRIX_HPP__

#include <iostream>
#include <cstring>
#include <iomanip>
#include "common.hpp"

namespace Math {

    template <typename T = double>
    class Matrix final {

        T*  data;

        int rowNum;
        int colNum;

        struct MatrixElem {

            int _col = 0;
            int _row = 0;

            T   _val {};

            void submatrMaxElem (const int border, const int size, Matrix& matr) {

                _val = matr.getElemUsingRowCol (border, border);
                _row = border;
                _col = border;

                for (int i = border; i < size; ++i) {
                    
                    for (int j = border; j < size; ++j) {

                        T curElem = matr.getElemUsingRowCol (i, j);
                        if (DblCmp (std::abs(curElem), std::abs(_val)) > 0) {

                            _row = i;
                            _col = j;
                            _val = curElem;

                        }

                    }

                }

            }

        };

    private:

        int CheckCorrectRowRequest (const int row) const {

            if (row >= rowNum) {

                std::cout   << "Max row number is "  << rowNum - 1
                            << " but you requested " << row << std::endl;
                return -1;

            }

            return 0;
            
        }

        int CheckCorrectColRequest (const int col) const {

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
        
        void GaussRowTransform (const int i, const int j) {

            T elem3 = getElemUsingRowCol (j, i);
            T elem4 = getElemUsingRowCol (i, i);
            for (int curMoveCol = 0; curMoveCol < colNum; ++curMoveCol) {
                
                T elem1 = getElemUsingRowCol (j, curMoveCol);
                T elem2 = getElemUsingRowCol (i, curMoveCol);

                setElemUsingRowCol (j, curMoveCol, elem1 - elem2 * (elem3 / elem4));

            }
            
        }

        T calcUsingGaussMeth () const {

            Matrix<double> tmpSave {*this};
    
            int sign = tmpSave.makeTr ();

            T det = tmpSave.calcTrace ();

            return det * sign;

        }

        void eliminate (const int curInd, const int size) {

            double coef = 0;

            for (int i = curInd + 1; i < size; ++i) {

                coef = getElemUsingRowCol (i, curInd) / getElemUsingRowCol (curInd, curInd);

                for (int j = 0; j < size; ++j) {

                    T curElem       = getElemUsingRowCol (curInd, j);
                    double newVal   = coef * curElem;

                    T recalcElem    = getElemUsingRowCol (i, j);
                    setElemUsingRowCol (i, j, recalcElem - newVal);

                    if (!DblCmp (getElemUsingRowCol (i, j), 0))
                        setElemUsingRowCol (i, j, {});

                }

            }
            
        }

    public:
        //Constructor
        Matrix (int rN, int cN): 
            rowNum (rN),
            colNum (cN) {
            
            int fullSize = rowNum * colNum;

            if (fullSize > 0)
                data = new T [fullSize] {};
            else
                data = nullptr;

        }

        Matrix (int n = 0):
            rowNum (n),
            colNum (n) {
            
            if (n > 0)
                data = new T [n * n] {};
            else
                data = nullptr;

        }

        //Copy constructor
        Matrix (const Matrix &toCopy):
            rowNum      (toCopy.rowNum),
            colNum      (toCopy.colNum) {

            int fullSize = rowNum * colNum;
            
            if (fullSize > 0)
                data = new T [fullSize] {};
            else
                data = nullptr;

            std::copy (toCopy.data, toCopy.data + fullSize, data);

        }

        template <typename U>
        Matrix (const Matrix<U> &toCopy) : 
            rowNum      (toCopy.getRowNum ()),
            colNum      (toCopy.getColNum ()) {

            int fullSize = rowNum * colNum; 
            
            if (fullSize > 0)
                data = new T [fullSize] {};
            else
                data = nullptr;

            for (int i = 0; i < rowNum * colNum; ++i)
                data[i] = static_cast<T> (toCopy [i]);

        }

        //Copy assignments
        Matrix& operator= (const Matrix& toCopy) {

            if (this == &toCopy)    //scary and cold
                return *this;

            rowNum = toCopy.rowNum;
            colNum = toCopy.colNum;

            int fullSize    = rowNum * colNum;
            
            if (fullSize > 0) {

                T* dataTmp      = new T [fullSize];
                std::copy (toCopy.data, toCopy.data + fullSize, dataTmp);

                delete[] data;
                data = dataTmp;
            
            } else {

                delete[] data;
                data = nullptr;

            }

            return *this;

        }

        //Move constructor
        Matrix (Matrix<T> &&toMove) noexcept :
            data (toMove.data),
            rowNum (toMove.rowNum),
            colNum (toMove.colNum) {

            toMove.data = nullptr;  //stolen -_-

        }

        //Move assignment
        Matrix& operator= (Matrix<T> &&toMove) noexcept {

            if (this == &toMove)    //bad-bad things...
                return *this;
            
            std::swap (toMove.data, data);
            colNum = toMove.colNum;
            rowNum = toMove.rowNum;

            return *this;

        }

        //Destructor
        ~Matrix () {
            delete[] data;
        }

        int getRowNum () const {
            return rowNum;
        }

        int getColNum () const {
            return colNum;
        }

        T& operator[] (const int ind) {
            return data [ind];
        }

        const T& operator[] (const int ind) const {
            return data [ind];
        }

        //PseudoOperators
        T getElemUsingRowCol (const int row, const int col) const {

            // The motivation for creating this function is that
            //I don't want to create proxy structures, cause it looks
            //more cache friendly when we use T* data as a straight
            //array. Maybe I'm wrong.

            if (CheckCorrectRowRequest (row) != 0) return T {};
            if (CheckCorrectColRequest (col) != 0) return T {};

            return data [row * colNum + col];

        }

        void setElemUsingRowCol (const int row, const int col, T elem) {

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

                    std::cout << std::setw (10) << data [curBegInd + curCol];
                    if (curCol + 1 < colNum)
                        std::cout << " ";
            
                }
                std::cout << "]" << std::endl;

            }

        }

        void swapRows (const int first, const int second, const int size) {

            int swapRowBeg = size * first;
            int swapRowEnd = swapRowBeg + size;

            std::swap_ranges (data + swapRowBeg, data + swapRowEnd, data + second * size);

        }

        void swapColumns (const int first, const int second, const int size) {

            for (int i = 0; i < size; ++i)
                std::swap (data [i * size + first], data [i * size + second]);

        }

        T calcTrace () const {

            T det {1};

            for (int i = 0; i < rowNum; ++i) {
                
                T curElem = getElemUsingRowCol (i, i);
                if (DblCmp (curElem, 0))
                    det = det * curElem;
                else 
                    return T {0};

            }

            return det;

        }

        T calcDet () const {
                
            if (rowNum != colNum) {

                std::cout << "I dunno how to calculate non-square matrix's determinate :(" << std::endl;
                return T {};

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

        int makeTr () {

            int sign = 1;
            int size = rowNum; //renaming
            for (int i = 0; i < size; ++i) {
                
                MatrixElem maxElem {};
                maxElem.submatrMaxElem (i, size, *this);

                if (maxElem._col != i) {

                    swapColumns (i, maxElem._col, size);

                    sign *= -1;
                    maxElem._col = i;

                }

                if (maxElem._row != i) {

                    swapRows (i, maxElem._row, size);

                    sign *= -1;
                    maxElem._row = i;

                }

                if (DblCmp (getElemUsingRowCol (maxElem._row, maxElem._col), 0) == 0)
                    return 0;

                eliminate (i, size);

            }

            return sign;

        }

    };

}

#endif
