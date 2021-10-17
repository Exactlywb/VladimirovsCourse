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

        pType getAbsMaxCoord () const;

        bool isTriangle () const;

        bool signedDistance (Plane &plain);

		void calcNormal (Vector &normalVector) const;

		void calcCoefD (Vector &normalV, pType &ourCoefD) const;

        bool isIntersected (Triangle &tr);
	
		bool pointInTriangle (const Vector &point) const;
    };

	//##############################################################################
	//                         TRIANGLE-OVERLOAD PART
	//##############################################################################

    std::istream &operator >> (std::istream &in, Triangle &triangle);
    std::ostream &operator << (std::ostream &out, const Triangle &triangle);

    //##############################################################################
    //                         TRIANGLE-3D INTERSECTION (Frolov)
    //##############################################################################

    void CountCommonP  (pType firstD, pType secondD, Vector& firstNormalVec,
                        Vector& secondNormalVec, Vector& commonP);

    void ProjectEdges  (pType projection [3], const Triangle& tr,
                        const Vector& leadVec, const Vector& commonP);

    bool IsIntersectedTIntervals (pType firstTParams [2], pType secondTParams [2]);

    pType CalcDist (const Vector& normalV, const pType dCoef, const Vector& point);

    void CalcTParams (pType tParams [2], const pType projection [3],
                      const Vector& normalV, const pType dCoef, const Triangle& tr);

    bool Intersect3DTriangles (const Triangle& tr1, const Triangle& tr2);
    bool intersectSegments (const Vector& begin_1, const Vector& segment_1, const Vector& begin_2, const Vector& segment_2);

    bool intersect2DTriangles (const GObjects::Triangle &tr1, const GObjects::Triangle &tr2);
}

#endif
