#ifndef MATRIX_H__
#define MATRIX_H__

#include <iostream>
#include <exception>
#include <iomanip>

namespace Matrix {

    const double Epsilon = 1e-5;

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
                
                delete [] data_;
                data_ = nullptr;
            
            }

        }

    public:

        //====================================================================================
        //========================== BIG FIVE RULE IMPLEMENTATION ============================
        //====================================================================================
        Matrix (const int nRows, const int nCols):
            nRows_ (nRows), nCols_ (nCols) {

            try {
                
                int size = nRows_ * nCols_;        
                if (size)
                    data_ = new T [size];
                else
                    data_ = nullptr;

            } catch (std::bad_alloc& err) {
               
                std::cout << "bad allocation for memory: " << err.what () << std::endl;
                data_ = nullptr;

            }

        }
    
        Matrix (Matrix<T> &&other) noexcept:
            data_ (other.data_),
            nRows_ (other.nRows_), nCols_ (other.nCols) { //move constructor
        
            other.data_ = nullptr;

        }

        Matrix (const Matrix<T> &other):
            nRows_ (other.nRows_), nCols_ (other.nCols_) { //copy constructor
            
            int size = nRows_ * nCols_;
            try {

                if (size)
                    data_ = new T [size];
                else
                    data_ = nullptr;

            } catch (std::bad_alloc& err) {
        
                std::cout << "bad allocation for memory: " << err.what () << std::endl;
                data_  = nullptr;
                return;
        
            }

            copyMatrix (other, size);

        }

        Matrix& operator= (Matrix &&other) { //move assignment

            if (this == &other)
                return *this;

            delete [] data_;
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

            delete [] data_;
            
            int size = nRows_ * nCols_;
            try {

                if (size)
                    data_ = new T [size];
                else
                    data_ = nullptr;

            } catch (std::bad_alloc& err) {
                
                std::cout << "bad memory allocation: " << err.what () << std::endl;
                data_ = nullptr;
                return *this;

            }

            copyMatrix (other, size);

        }

        ~Matrix () {

            delete [] data_;
            // Here is no reason to catch exceptions after delete.
            // We don't know what kind of exception can be throwed
            //after T destructor.

        } 
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

        void textDump () const {

            for (int y = 0; y < nRows_; ++y) {
                
                for (int x = 0; x < nCols_; ++x)
                    std::cout << std::setw (5) << data_ [y * nCols_ + x];
                
                std::cout << std::endl;

            }

        }

    };  

}

#endif

