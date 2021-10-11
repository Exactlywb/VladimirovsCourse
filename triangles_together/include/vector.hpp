#ifndef VECTOR_H__
#define VECTOR_H__

#include <iostream>
#include <algorithm>

namespace GObjects {

    template <typename pType = double>
    class Vector {

        pType coordinates_ [3];

    public:
        Vector (pType x = 0, pType y = 0, pType z = 0):
                coordinates_ {x, y, z} {}

        pType getCoord (int coordNum) const {
            if (coordNum > 2) {
                return 0;
            }

            return coordinates_ [coordNum];
        }

        pType getAbsMaxCoord () const {
            return std::max ({(const pType)std::abs (coordinates_ [0]), 
                              (const pType)std::abs (coordinates_ [1]), 
                              (const pType)std::abs (coordinates_ [2])});
        }

        void setCoord (const int coordNum, pType coord) {

            coordinates_ [coordNum] = coord;

        }

        pType squareLength () const;

    };

    template <typename pType = double>
    Vector<pType> operator - (const Vector<pType>& v) {

        return Vector<pType>    (-v.getCoord (0),
                                 -v.getCoord (1),
                                 -v.getCoord (2));

    }

    template <typename pType = double>
    Vector<pType> operator + (const Vector<pType>& firstV, const Vector<pType>& secondV) {

        Vector<pType> newVector     (firstV.getCoord (0) + secondV.getCoord (0),
                                     firstV.getCoord (1) + secondV.getCoord (1),
                                     firstV.getCoord (2) + secondV.getCoord (2));

        return newVector;

    }

    template <typename pType = double>
    Vector<pType> operator - (const Vector<pType>& firstV, const Vector<pType>& secondV) {

        Vector<pType> newVector     (firstV.getCoord (0) - secondV.getCoord (0),
                                     firstV.getCoord (1) - secondV.getCoord (1),
                                     firstV.getCoord (2) - secondV.getCoord (2));

        return newVector;

    }

    template <typename pType = double>
    Vector<pType> operator * (pType mulN, const Vector<pType>& v) {

        Vector<pType> newVector (mulN * v.getCoord (0),
                                 mulN * v.getCoord (1),
                                 mulN * v.getCoord (2));

        return newVector;
        
    }

    template <typename pType = double>
    pType operator * (const Vector<pType>& firstV, const Vector<pType>& secondV) {

        pType resScal = firstV.getCoord (0) * secondV.getCoord (0) +
                        firstV.getCoord (1) * secondV.getCoord (1) +
                        firstV.getCoord (2) * secondV.getCoord (2);

        return resScal;
        
    }

    template <typename pType>
    pType Vector<pType>::squareLength () const {

        const Vector<pType>* thisRef = this;
        return (*thisRef) * (*thisRef);
            
    }

    template <typename pType = double>
    Vector<pType> operator ^ (const Vector<pType>& firstV, const Vector<pType>& secondV) {
        
        Vector<pType> newVector (firstV.getCoord (1) * secondV.getCoord (2) - firstV.getCoord (2) * secondV.getCoord (1),
                                 firstV.getCoord (2) * secondV.getCoord (0) - firstV.getCoord (0) * secondV.getCoord (2),
                                 firstV.getCoord (0) * secondV.getCoord (1) - firstV.getCoord (1) * secondV.getCoord (0));

        return newVector;

    }

    template <typename pType = double>
    std::ostream& operator << (std::ostream &out, const Vector<pType>& thou) {

        out << "(" << thou.getCoord (0) << ", " << thou.getCoord (1) << ", " << thou.getCoord (2) << ")";
        return out;

    }


    template <typename pType = double>
    std::istream& operator >> (std::istream &in, Vector<pType>& thou) {

        pType newX;
        pType newY;
        pType newZ;

        in >> newX >> newY >> newZ;

        thou.setCoord (0, newX);
        thou.setCoord (1, newY);
        thou.setCoord (2, newZ);

        return in;

    }

    template <typename pType = double>
    bool operator == (const Vector<pType>& firstV, const Vector<pType>& secondV) {

        if ((firstV.getCoord (0) - secondV.getCoord (0)) < __DBL_EPSILON__ &&
            (firstV.getCoord (1) - secondV.getCoord (1)) < __DBL_EPSILON__ &&
            (firstV.getCoord (2) - secondV.getCoord (2)) < __DBL_EPSILON__)
            return 1;
        
        return 0;

    }

}

#endif