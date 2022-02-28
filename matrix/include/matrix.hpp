#ifndef MATRIX_H__
#define MATRIX_H__

#include <iostream>
#include <exception>
#include <iomanip>

namespace Matrix {

    const double Epsilon = 1e-10;

    template <typename T = double>
    class Matrix final {

    protected:
        int nRows_;
        int nCols_;

        T*  data_;
    
    private:
        void copyMatr (const Matrix& other, const int size) {

        
            try {
                std::copy (other.data_, other.data_ + size, data_);
            } catch (std::bad_alloc& err) {
            
                std::cout << "bad allocation in std::copy algorithm: " << err.what ()<< std::endl;
                
                ::operator delete (data_);
                data_ = nullptr;
            
            }

        }

    public:

        //====================================================================================
        //========================== BIG FIVE RULE IMPLEMENTATION ============================
        //====================================================================================
        Matrix (const int nRows, const int nCols):
            nRows_ (nRows), nCols_ (nCols) {

            int size = nRows_ * nCols_;
            if (size)
                data_ = static_cast<T*> (::operator new (sizeof (T) * size));
            else
                data_ = nullptr;        

        }
    
        Matrix (Matrix<T> &&other) noexcept:
            data_ (other.data_),
            nRows_ (other.nRows_), nCols_ (other.nCols) { //move constructor
        
            other.data_ = nullptr;

        }

        Matrix (const Matrix<T> &other):
            nRows_ (other.nRows_), nCols_ (other.nCols_) { //copy constructor
            
            int size = nRows_ * nCols_;
            if (size)
                data_ = static_cast<T*> (::operator new (sizeof (T) * size));
            else
                data_ = nullptr;

            copyMatr (other, size);

        }

        Matrix& operator= (Matrix &&other) { //move assignment

            if (this == &other)
                return *this;

            ::operator delete (data_);
            data_ = other.data_;
            other.data_ = nullptr;
            
            nRows_ = other.nRows_;
            nCols_ = other.nCols_;

            return *this;

        }

        Matrix& operator= (const Matrix& other) { //copy assignment

            if (this == &other)
                return *this;

            nRows_ = other.nRows_;
            nCols_ = other.nCols_;

            ::operator delete (data_);
            
            int size = nRows_ * nCols_;
            if (size)
                data_ = static_cast<T*> (::operator new (sizeof (T) * size));
            else
                data_ = nullptr;

            copyMatrix (other, size);

        }

        ~Matrix () {

            ::operator delete (data_);
            // Here is no reason to catch exceptions after delete.
            // We don't know what kind of exception can be throwed
            //after T destructor.

        } 
        //====================================================================================

        //====================================================================================
        //=========================== operator[] implementation ==============================
        //====================================================================================
        struct Proxy {

            T* row_;
            int nCols_;

            Proxy (T* row, const int nCols):
                row_ (row), nCols_ (nCols) {}

            const T& operator[] (const int col) const {

                if (col > nCols_)
                    throw std::runtime_error ("bad column number to get element");

                return row_ [col]; 

            }

            T& operator[] (const int col) { 
            
                if (col > nCols_)
                    throw std::runtime_error ("bad column number to get element");

                return row_ [col]; 
            }
    
        };

        Proxy operator[] (int row) {

            Proxy ret {data_ + row * nCols_, nCols_};
            return ret;

        }
        //====================================================================================

        void textDump () const {

            for (int y = 0; y < nRows_; ++y) {
                
                for (int x = 0; x < nCols_; ++x)
                    std::cout << std::setw (5) << data_ [y * nCols_ + x];
                
                std::cout << std::endl;

            }

        }

        T det () const {

            Matrix<T> copyMatr (*this);
            double det = copyMatr.Gauss ();

            return det;

        }        

        double calcDiagMul () const {

            if (nCols_ != nRows_)
                throw std::runtime_error ("not a square matrix to evaluate diagonal multiplication");

            double diagMul {1};

            int size = nCols_;
            for (int i = 0; i < nCols_; ++i)
                diagMul *= data_ [i * (nCols_ + 1)];
    
            return diagMul;

        }

    protected:

        double Gauss () {    //|!| THIS FUNCTION DESTROYS MATRIX

            if (nCols_ != nRows_)
                std::runtime_error ("not a square matrix to evaluate determinant");

            int sgn = 1;
            for (int curIt = 0; curIt < nCols_; ++curIt) {

                //row, column
                std::pair<int, int> curMaxElem = maxSubmatrixElem (curIt);
                
                if (curMaxElem.second != curIt) {
                    
                    swapCols (curIt, curMaxElem.second);
                    sgn *= -1;
                    curMaxElem.second = curIt;

                }

                if (curMaxElem.first != curIt) {

                    swapRows (curIt, curMaxElem.first);
                    sgn *= -1;
                    curMaxElem.first = curIt;

                }

                if (std::abs (data_ [curIt * (nCols_ + 1)]) < Epsilon)
                    return T {};
                
                eliminate (curIt);

            }

            double determinant = calcDiagMul ();
            return sgn * determinant;

        }

    private:
        std::pair<int, int> maxSubmatrixElem (const int border) { 
        
            T curMax = data_ [border * (nCols_ + 1)];
            std::pair<int, int> res {border, border};
            for (int y = border; y < nRows_; ++y) {

                for (int x = border; x < nCols_; ++x) {

                    T toCmp = data_ [y * nCols_ + x];
                    if (std::abs (curMax) - std::abs (toCmp) < Epsilon &&
                        std::abs (toCmp) >= Epsilon) {
                        
                        res.first   = y;
                        res.second  = x;

                        curMax      = toCmp;

                    }

                }

            }

            return res;
        
        }
        
        void swapCols (const int border, const int col) {

            for (int row = border; row < nRows_; ++row) {

                std::swap (data_ [row * nCols_ + border],
                           data_ [row * nCols_ + col]);

            }

        }

        void swapRows (const int border, const int row) {

            for (int col = border; col < nCols_; ++col) {

                std::swap (data_ [border * nCols_ + col],
                           data_ [row    * nCols_ + col]);                

            }

        }

        void eliminate (const int border) {

            double coeff {};
            for (int row = border + 1; row < nRows_; ++row) {

                coeff = (double)data_ [row * nCols_ + border] / data_ [border * (nCols_ + 1)];

                for (int col = 0; col < nCols_; ++col) {

                    int pos = row * nRows_ + col;
                    data_ [pos] -= data_ [border * nCols_ + col] * coeff;
                    if (std::abs (data_ [pos]) < Epsilon)
                        data_ [pos] = T {};

                }
            
            }

        }

    };

}

#endif

