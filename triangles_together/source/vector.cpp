#include "../include/vector.hpp"

namespace GObjects {
	Vector::Vector (pType x, pType y, pType z):
                coordinates_ {x, y, z} {}

    pType Vector::getCoord (int coordNum) const {
		if (coordNum > 2) {
			return 0;
		}

		return coordinates_ [coordNum];
	}

	pType Vector::getAbsMaxCoord () const {
		return std::max ({(const pType)std::abs (coordinates_ [0]), 
						  (const pType)std::abs (coordinates_ [1]), 
						  (const pType)std::abs (coordinates_ [2])});
	}

	void Vector::setCoord (const int coordNum, pType coord) {
		coordinates_ [coordNum] = coord;
	}

    pType Vector::squareLength () const {
        const Vector* thisRef = this;
        return (*thisRef) * (*thisRef);
    }

    Vector operator - (const Vector &v) {

        return Vector  (-v.getCoord (0),
                        -v.getCoord (1),
                        -v.getCoord (2));

    }

    Vector operator + (const Vector &firstV, const Vector &secondV) {

        Vector newVector   (firstV.getCoord (0) + secondV.getCoord (0),
                            firstV.getCoord (1) + secondV.getCoord (1),
                            firstV.getCoord (2) + secondV.getCoord (2));

        return newVector;

    }

    Vector operator - (const Vector &firstV, const Vector &secondV) {

        Vector newVector   (firstV.getCoord (0) - secondV.getCoord (0),
                            firstV.getCoord (1) - secondV.getCoord (1),
                            firstV.getCoord (2) - secondV.getCoord (2));

        return newVector;

    }

    Vector operator * (pType mulN, const Vector &v) {

        Vector newVector   (mulN * v.getCoord (0),
                        	mulN * v.getCoord (1),
                            mulN * v.getCoord (2));

        return newVector;
        
    }
	
    Vector operator / (const Vector &v, pType divN) {

        Vector newVector (v.getCoord (0) / divN,
                                 v.getCoord (1) / divN,
                                 v.getCoord (2) / divN);

        return newVector;
        
    }

    pType operator * (const Vector &firstV, const Vector &secondV) {

        pType resScal = firstV.getCoord (0) * secondV.getCoord (0) +
                        firstV.getCoord (1) * secondV.getCoord (1) +
                        firstV.getCoord (2) * secondV.getCoord (2);

        return resScal;
        
    }
    
    Vector operator ^ (const Vector &firstV, const Vector &secondV) {
        
        Vector newVector   (firstV.getCoord (1) * secondV.getCoord (2) - firstV.getCoord (2) * secondV.getCoord (1),
                            firstV.getCoord (2) * secondV.getCoord (0) - firstV.getCoord (0) * secondV.getCoord (2),
                            firstV.getCoord (0) * secondV.getCoord (1) - firstV.getCoord (1) * secondV.getCoord (0));

        return newVector;

    }

    
    std::ostream& operator << (std::ostream &out, const Vector &thou) {

        out << "(" << thou.getCoord (0) << ", " << thou.getCoord (1) << ", " << thou.getCoord (2) << ")";
        return out;

    }


    
    std::istream& operator >> (std::istream &in, Vector &thou) {

        pType newX;
        pType newY;
        pType newZ;

        in >> newX >> newY >> newZ;

        thou.setCoord (0, newX);
        thou.setCoord (1, newY);
        thou.setCoord (2, newZ);

        return in;

    }

    
    bool operator == (const Vector &firstV, const Vector &secondV) {

        if (std::abs((firstV.getCoord (0) - secondV.getCoord (0))) < __DBL_EPSILON__ &&
            std::abs((firstV.getCoord (1) - secondV.getCoord (1))) < __DBL_EPSILON__ &&
            std::abs((firstV.getCoord (2) - secondV.getCoord (2))) < __DBL_EPSILON__)
            return 1;
        
        return 0;

    }
}