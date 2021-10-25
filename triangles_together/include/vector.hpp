#ifndef VECTOR_H__
#define VECTOR_H__

#include <iostream>
#include <algorithm>
#include "common.hpp"

namespace GObjects {
    using pType = double;

    class Vector {

        pType coordinates_ [3];

    public:
        Vector (pType x = 0, pType y = 0, pType z = 0):
                coordinates_ {x, y, z} {}

//-----------------------------------------------------------------------------------------------------

        pType getCoord (int coordNum) const {
            if (coordNum > 2 || coordNum < 0)
                return 0;
            
            return coordinates_ [coordNum];
        }

//-----------------------------------------------------------------------------------------------------

        pType getAbsMaxCoord () const;
        pType getAbsMinCoord () const;

//-----------------------------------------------------------------------------------------------------

        void setCoord (const int coordNum, pType coord) {
            if (coordNum > 2 || coordNum < 0)
                return;
            
            coordinates_ [coordNum] = coord;
        }

//-----------------------------------------------------------------------------------------------------

        pType squareLength () const;

//-----------------------------------------------------------------------------------------------------

        static const Vector getZeroVector () {
            const static Vector zeroVec {};
            return zeroVec;
        }
    };

    const Vector GetZeroVector ();

    Vector operator -   (const Vector &v);
    Vector operator +   (const Vector &firstV, const Vector &secondV);
    Vector operator -   (const Vector &firstV, const Vector &secondV);
    Vector operator *   (pType mulN, const Vector &v);
    Vector operator /   (const Vector &v, pType divN);

    pType operator *    (const Vector &firstV, const Vector &secondV);
    Vector operator ^   (const Vector &firstV, const Vector &secondV);

    std::ostream& operator <<   (std::ostream &out, const Vector &thou);
    std::istream& operator >>   (std::istream &in, Vector &thou);

    bool operator ==            (const Vector &firstV, const Vector &secondV);
    
    double determinant                  (const Vector &first, const Vector &second,
                                         const Vector &third);
}


#endif
