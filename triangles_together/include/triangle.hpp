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
#include "common.hpp"

namespace GObjects {
//***
//*  Actually, it makes sense to give opportunity to handle
//* of the degradated cases to the classes themselves. Some geometric
//* objects may not be interested in that. Anothers may handle it by
//* another way.
//***
// enum DegenerateType {

//     NONE_DEG,
//     DEG_POINT,
//     DEG_SEGMENT

// };

//##############################################################################
//                         PLANE-CLASS PART
//##############################################################################
    struct Segment;
    
    
    class Plane {
          Vector nVec_;
          pType d_;

  	public:
        Plane (const Vector &vec, const pType d): 
               nVec_ (vec),
               d_ {d} {}

        Plane ():
                nVec_ {},
                d_ {} {}

        Vector getVec () const {
            return nVec_;
        }

        pType getD () const {
            return d_;
        }

        pType dist (Vector &vec) const;
    };

    std::ostream &operator << (std::ostream &out, const Plane &plane);

//=====================================================================================================

    class Triangle {

    private:
        Vector rVecs_[3] {};
        char typeOfDegeneration_;   // 1 is not degenerate;
                                    // 2 is a point
                                    // 4 is a segment
        int number_;

    public:
        Triangle ():
            rVecs_ {} {
            typeOfDegeneration_ = 2;
        }

        Triangle    (const Vector &vec1, const Vector &vec2, 
                    const Vector &vec3):
                    rVecs_ {vec1, vec2, vec3} {
            typeOfDegenerate ();
        }

        void setVec (Vector &vec, int num) {
            rVecs_ [num] = vec;
            typeOfDegenerate ();
        }

        Vector getVec (int num) const {
            return rVecs_ [num];
        }

        int getNumber () const {
            return number_;
        }

        void setNumber (const int number) {
            number_ = number;
        }

        pType getAbsMaxCoord () const;
        pType getAbsMinCoord () const;

        void typeOfDegenerate     ();
        char getDegenerationType  () const {
            return typeOfDegeneration_;
        }

        char signedDistance (const Plane &plain, const Triangle &tr) const;

        void calcNormal (Vector &normalVector) const;
        void calcCoefD (Vector &normalV, pType &ourCoefD) const;

        bool pointInTriangle (const Vector &point) const;
    };

    //##############################################################################
    //                         TRIANGLE-OVERLOAD PART
    //##############################################################################

    std::istream &operator >> (std::istream &in, Triangle &triangle);
    std::ostream &operator << (std::ostream &out, const Triangle &triangle);

    //##############################################################################
    //                         TRIANGLE-3D INTERSECTION
    //##############################################################################

    void CountCommonP  (pType firstD, pType secondD, Vector& firstNormalVec,
                        Vector& secondNormalVec, Vector& commonP);

    void ProjectEdges  (pType projection [3], const Triangle& tr,
                        const Vector& leadVec, const Vector& commonP);

    bool IsIntersectedTIntervals (pType firstTParams [2], pType secondTParams [2]);

    pType   CalcDist    (const Vector& normalV, const pType dCoef, const Vector& point);

    void CalcTParams (pType tParams [2], const pType projection [3],
                      const Vector& normalV, const pType dCoef, const Triangle& tr);

    bool Intersect3DTriangles (const Triangle& tr1, const Triangle& tr2);
    bool IntersectSegments (const Segment& segment_1, const Segment& segment_2);

    bool Intersect2DTriangles (const Triangle &tr1, const Triangle &tr2);
    Vector IntersectionPointOfTwoLines (const Vector &begin_1, const Vector &segment_1, 
                                        const Vector &segment_2, const Vector &segment_3, 
                                        const Vector &difVec);


//=====================================================================================================

    struct Segment {
        Vector begin_, direct_;

        Segment (const Vector begin = 0, const Vector direct = 0) :
            begin_ {begin},
            direct_ {direct} {}

        Segment (const Triangle &tr) {
            Vector side1 = tr.getVec(0) - tr.getVec(1);
            Vector side2 = tr.getVec(2) - tr.getVec(1);

            double scalarProduct = (side1 * side2);

            double denominator = std::sqrt(side1.squareLength()) * std::sqrt(side2.squareLength());

            if (DoubleCmp((scalarProduct / denominator), -1.0) == 0) {
                begin_ = tr.getVec(0);
                direct_ = tr.getVec(2) - tr.getVec(0);
            }
            else {
                begin_ = tr.getVec(1);
                if (DoubleCmp(side1.squareLength(), side2.squareLength()) > 0)
                    direct_ = side1;
                else 
                    direct_ = side2;
            }
        }
    };

//=====================================================================================================

    bool IntersectDegenerates (const Triangle &tr, const Vector &point);
    bool IntersectDegenerates (const Triangle &tr, const Segment &segment);
    bool IntersectDegenerates (const Segment &segment1, const Segment &segment2);
    bool IntersectDegenerates (const Segment &segment, const Vector &point);
    bool IntersectDegenerates (const Vector &point1, const Vector &point2);

}
#endif
