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

//=====================================================================================================

    class Triangle {

    private:
          Vector rVecs_[3] {};
            char typeOfDegeneration_;   // 1 is not degenerate;
                                        // 2 is a point
                                        // 4 is a segment

    public:
        Triangle ();

        Triangle (const Vector &vec1, const Vector &vec2, const Vector &vec3);

        void setVec (Vector &vec, int num);

        Vector getVec (int num) const;

        pType getAbsMaxCoord () const;
        pType getAbsMinCoord () const;

        void typeOfDegenerate     ();
        char getDegenerationType  () const;

        bool signedDistance (const Plane &plain) const;

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
    //                         TRIANGLE-3D INTERSECTION (Frolov)
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
    bool IntersectSegments    (const Vector& begin_1, const Vector& segment_1, const Vector& begin_2, const Vector& segment_2);

    bool Intersect2DTriangles (const Triangle &tr1, const Triangle &tr2);
    Vector IntersectionPointOfTwoLines (const Vector &begin_1, const Vector &segment_1, 
                                        const Vector &segment_2, const Vector &segment_3, 
                                        const Vector &difVec);


//=====================================================================================================

    struct Segment {
        Vector begin_, direct_;

        Segment (const Triangle &tr) {
            Vector side1 = tr.getVec(0) - tr.getVec(1);
            Vector side2 = tr.getVec(2) - tr.getVec(1);

            double scalarProduct = (side1 * side2);

            double denominator = std::sqrt(side1.squareLength()) * std::sqrt(side2.squareLength());

            // std::cout << "scalarProduct = " << scalarProduct << std::endl;
            // std::cout << "denomin = " << denominator << std::endl;

            // std::cout << "How I hate it" << scalarProduct / denominator << std::endl;

            if (DoubleCmp((scalarProduct / denominator), -1.0) == 0) {

                begin_ = tr.getVec(0);
                direct_ = tr.getVec(2) - tr.getVec(0);
            }
            else {
                begin_ = tr.getVec(1);
                if (side1.squareLength() > side2.squareLength())
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
