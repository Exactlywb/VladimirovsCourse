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
    class Triangle;
    class Plane;
    struct Segment;
    
//=====================================================================================================

    class Triangle final {

    private:
        Vector rVecs_[3] {};
        char typeOfDegeneration_;   // 1 is not degenerate;
                                    // 2 is a point
                                    // 4 is a segment
        int number_;

    public:
        Triangle (): 
            typeOfDegeneration_ (2) {}

//-----------------------------------------------------------------------------------------------------

        Triangle    (const Vector &vec1, const Vector &vec2, const Vector &vec3):
                    rVecs_ {vec1, vec2, vec3} {
            
            typeOfDegenerate ();
        }

//-----------------------------------------------------------------------------------------------------

        void setVec (Vector &vec, int num) noexcept {
            rVecs_ [num] = vec;
            typeOfDegenerate ();
        }

//-----------------------------------------------------------------------------------------------------

        Vector getVec (int num) const noexcept {
            return rVecs_ [num];
        }

//-----------------------------------------------------------------------------------------------------

        int getNumber () const noexcept {
            return number_;
        }

//-----------------------------------------------------------------------------------------------------

        void setNumber (const int number) noexcept {
            number_ = number;
        }

//-----------------------------------------------------------------------------------------------------

        pType getAbsMaxCoord () const noexcept;
        pType getAbsMinCoord () const noexcept;

//-----------------------------------------------------------------------------------------------------

        void typeOfDegenerate     () noexcept;
        char getDegenerationType  () const noexcept {
            return typeOfDegeneration_;
        }

//-----------------------------------------------------------------------------------------------------

        char signedDistance (const Plane &plain, const Triangle &tr) const;

        void calcNormal (Vector &normalVector) const;
        void calcCoefD (Vector &normalV, pType &ourCoefD) const noexcept;

        bool pointInTriangle (const Vector &point) const;
    };

//-----------------------------------------------------------------------------------------------------

    std::istream &operator >> (std::istream &in, Triangle &triangle);
    std::ostream &operator << (std::ostream &out, const Triangle &triangle);

//-----------------------------------------------------------------------------------------------------

    bool Intersect3DTriangles (const Triangle& tr1, const Triangle& tr2);

//##############################################################################
//                         PLANE-CLASS PART
//##############################################################################

    class Plane final {
          Vector nVec_;
          pType d_;

    public:
        Plane ():
            nVec_ {},
            d_ {} {}

//-----------------------------------------------------------------------------------------------------

        Plane (const Vector &vec, const pType d): 
               nVec_ (vec),
               d_ {d} {}

//-----------------------------------------------------------------------------------------------------

        Plane (const Triangle &tr) {
            tr.calcNormal(nVec_);
            tr.calcCoefD(nVec_, d_);
        }

//-----------------------------------------------------------------------------------------------------

        Vector getVec () const noexcept {
            return nVec_;
        }

//-----------------------------------------------------------------------------------------------------

        pType getD () const noexcept {
            return d_;
        }

//-----------------------------------------------------------------------------------------------------

        pType dist (Vector &vec) const noexcept;    // for the future
    };

    std::ostream &operator << (std::ostream &out, const Plane &plane);


//=====================================================================================================

    struct Segment final {
        Vector begin_, direct_;

//-----------------------------------------------------------------------------------------------------

        Segment (const Vector begin = 0, const Vector direct = 0) :
            begin_ {begin},
            direct_ {direct} {}

//-----------------------------------------------------------------------------------------------------

        Segment (const Triangle &tr) {

            Cmp::cmp DoubleCmp;
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

}
#endif
