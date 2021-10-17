#ifndef TRIANGLE_H__
#define TRIANGLE_H__

#include <iostream>
#include <new>
#include <iterator>
#include <cassert>
#include <algorithm>
#include <list>
#include <cmath>
#include "vector.hpp"

namespace GObjects {
    //##############################################################################
    //                         PLANE-CLASS PART
    //##############################################################################
	class Plane {
        Vector nVec_;
        pType d_;

    public:
        Plane (const Vector &vec, const pType d);

		Plane ();

        Vector getVec () const;
        pType getD () const;

        pType dist (Vector &vec) const;
    };

    std::ostream &operator << (std::ostream &out, const Plane &plane);

    class Triangle {

    private:
        Vector rVecs_[3] {};

    public:
		Triangle ();

        Triangle (const Vector &vec1, const Vector &vec2, const Vector &vec3);

        void setVec (Vector &vec, int num);

        Vector getVec (int num) const;

        pType getAbsMaxCoord ();
        
        bool isTriangle () const;

        bool signedDistance (Plane &plain);

		void CalcNormal (Vector &normalVector);

		void calcCoefD (Vector &normalV, pType &ourCoefD);

        bool isIntersected (Triangle &tr);
    };
	//##############################################################################
	//                         TRIANGLE-OVERLOAD PART
	//##############################################################################

    std::istream &operator >> (std::istream &in, Triangle &triangle);

    std::ostream &operator << (std::ostream &out, const Triangle &triangle);
}

#endif