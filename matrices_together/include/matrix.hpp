#ifndef MATRIX_H__
#define MATRIX_H__

#include <iostream>


namespace MathObj {

    template <typename elemT>
    class Matrix {
    
        int *data_; //for better cache-effect

        int xSize_;
        int ySize_;
    
    public:
        Matrix (int colN = 0, int rowN = 0, elemT vals = T{});
        
        template <typename It>
        Matrix (int colN, int rowN, It start, It fin);
        
        //copy and move construct
        Matrix (const Matrix &rhs);
        Matrix (Matrix &&rhs);

        //assignment and move
        Matrix& operator=(const Matrix &rhs);
        Matrix& operator=(Matrix &&rhs);

        struct ProxyMatrix {
        
            elemT *row;
            const elemT &operator[] (int n) const;
            elemT &operator[] (int n);

        };
        //[] operator
        ProxyMatrix operator[](int);
        
        //determinate
        template <typename resT>
        resT calcDeterminate ();

        //destructor
        ~Matrix ();
        
    };

}

#endif

