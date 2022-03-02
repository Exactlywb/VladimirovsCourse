#ifndef MATRIX_H__
#define MATRIX_H__

#include <iostream>
#include <exception>
#include <iomanip>
#include <algorithm>

namespace Matrix {

    template <typename From, typename To>
    struct StaticCasterCopier {

        To operator() (From fr) const { return static_cast<To> (new From (fr)); }

    };

    const double Epsilon = 1e-16;

    template <typename T = double>
    class Matrix final {

    protected:
        int nRows_;
        int nCols_;

        T*  data_;
    
    private:
        void copyMatr (const Matrix& other, const int size) {

            std::copy (other.data_, other.data_ + size, data_);

        }

        template <typename anotherT>
        void copyMatrAnother (const Matrix<anotherT>& other, const int size) {

            std::transform (other.getData (), other.getData () + size, data_, StaticCasterCopier<anotherT, T> ());

        }

    public:
        T* getData () const { return data_; }

        int getNRows () const { return nRows_; }
        int getNCols () const { return nCols_; }

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
            nRows_ (other.nRows_), nCols_ (other.nCols_) { //move constructor
        
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

            copyMatr (other, size);

            return *this;

        }

        //coercion constructor
        template <typename anotherT>
        Matrix (const Matrix<anotherT> &other): nRows_ (other.getNRows ()), nCols_ (other.getNCols ()) {

            int size = nRows_ * nCols_;
            if (size) {

                data_ = static_cast<T*> (::operator new (size * sizeof (T)));

                
            } else
                data_ = nullptr;

            copyMatrAnother (other, size);

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

            Matrix<double> copyMatr (*this);
            double det = copyMatr.Gauss ();

            return static_cast<T> (det);

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


        double Gauss () {    //|!| THIS FUNCTION DESTROYS MATRIX

            if (nCols_ != nRows_)
                throw std::runtime_error ("not a square matrix to evaluate determinant");

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

    template <typename T = double>
    bool operator== (const Matrix<T> &first, const Matrix<T> &second)
    {

        if (first.getNCols () != second.getNCols () ||
            first.getNRows () != second.getNRows ()) {
                return false; 
        }

        return std::equal (first.getData (), first.getData () + first.getNRows () * first.getNCols (), second.getData ());
    }

}

#endif

