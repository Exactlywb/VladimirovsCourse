#include "matrix.hpp"

namespace Math {

    template <typename T>
    void Matrix<T>::SetElemUsingRowNumAndColNum (const int col, const int row, T& elem) {

        // Similar with GetElemUsingRowNumAndColNum

        if (CheckCorrectRowRequest (row) != 0) return T {};
        if (CheckCorrectColRequest (col) != 0) return T {};

        data [row * col + col] = *elem;
        
    }

}
