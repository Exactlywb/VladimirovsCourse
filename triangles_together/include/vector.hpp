#ifndef VECTOR_H__
#define VECTOR_H__

#include <iostream>
#include <algorithm>

namespace GObjects {
	using pType = double;

    class Vector {

        pType coordinates_ [3];

    public:
        Vector (pType x = 0, pType y = 0, pType z = 0);

        pType getCoord (int coordNum) const;

        pType getAbsMaxCoord () const;

        void setCoord (const int coordNum, pType coord);

        pType squareLength () const;
    };

    Vector operator - (const Vector &v);
    Vector operator + (const Vector &firstV, const Vector &secondV);
    Vector operator - (const Vector &firstV, const Vector &secondV);
    Vector operator * (pType mulN, const Vector &v);
    Vector operator / (const Vector &v, pType divN);
    pType operator * (const Vector &firstV, const Vector &secondV);
    Vector operator ^ (const Vector &firstV, const Vector &secondV);
    std::ostream& operator << (std::ostream &out, const Vector &thou);
    std::istream& operator >> (std::istream &in, Vector &thou);
    bool operator == (const Vector &firstV, const Vector &secondV);
}

#endif