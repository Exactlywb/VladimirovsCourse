#pragma once

#include <iostream>
#include <new>
#include <iterator>
#include <cassert>
#include <algorithm>
#include <list>
#include <cmath>

namespace GObjects {

    //##############################################################################
    //                         PLANE-CLASS PART
    //##############################################################################
    template <typename PType> class Plane {
        Vector<pType> nVec_;
        pType d_;

    public:
        Plane (const Vector<pType> &vec, const pType d) :
            nVec_(vec),
            d_(d) {}

        Vector<pType> getVec () const {
            return nVec_;
        }
        pType getD () const {
            return d_;
        }

        pType dist (Vector<pType> &vec) const {
            pType dist{};

            for (int index = 0; index < 3; ++index) {

                dist += vec.getCoord(index) * nVec_(index);
            }

            return (dist + d_);
        }
    };

    template <typename pType>
    std::ostream &operator << (std::ostream &out, const Plane<pType> &plane) {    
        out << plane.getVec(); 
        out << "D = " << plane.getD() << std::endl; 
    
        return out;
    }

    template <typename pType> class Triangle {

    private:
        Vector<pType> rVecs_[3]{};

    public:       
        Triangle (const Vector<pType> &vec1, const Vector<pType> &vec2, const Vector<pType> &vec3) :
            rVecs_{vec1, vec2, vec3} {}

        void setVec (Vector<pType> &vec, int num) { 
            rVecs_[num] = vec;
        }

        Vector<pType> getVec (int num) const { 
            return rVecs_[num];
        }

        pType getAbsMaxCoord () {
            return (std::max ({(const pType) std::abs(rVecs_[0].getCoord(0)), (const pType) std::abs(rVecs_[0].getCoord(1)), (const pType) std::abs(rVecs_[0].getCoord(2)), 
                             (const pType) std::abs(rVecs_[1].getCoord(0)), (const pType) std::abs(rVecs_[1].getCoord(1)), (const pType) std::abs(rVecs_[1].getCoord(2)), 
                             (const pType) std::abs(rVecs_[2].getCoord(0)), (const pType) std::abs(rVecs_[2].getCoord(1)), (const pType) std::abs(rVecs_[2].getCoord(2))}));
        }   
        
        bool isTriangle () const {
            if ((rVecs[2] - rVecs[0] ^ rVecs[1] - rVecs[0]) == Vector<pType> ({0, 0, 0}))
                return false;
            else    
                return true;
        }

        bool signedDistance (Plane<pType> &plain) {
            pType dists[3]{};

            for (int i = 0; i < 3; ++i) {
                dists[i] = (rVecs_(i).getCoord(0) * plain.getVec().getCoord(0) + 
                            rVecs_(i).getCoord(1) * plain.getVec().getCoord(1) +
                            rVecs_(i).getCoord(2) * plain.getVec().getCoord(2) +
                            plain.getD(3));
            }
            return ((((long long) (dists[0]) & (long long) (dists[1]) & (long long) (dists[2])) | ~(((long long) (dists[0]) | (long long) (dists[1]) | (long long) (dists[2])))) 
                                    >> (sizeof(pType) * 8 - 1));
        }

    
        bool isIntersected (objects::Triangle<pType> &tr) {

            //TODO: For the future
        }
    };
        //##############################################################################
        //                         TRIANGLE-OVERLOAD PART
        //##############################################################################
        template <typename pType> 
        std::istream &operator >> (std::istream &in, Triangle<pType> &triangle) {

            Vector<pType> vec1;
            Vector<pType> vec2;
            Vector<pType> vec3;

            in >> vec1 >> vec2 >> vec3;

            triangle.setVec(vec1, 0); 
            triangle.setVec(vec2, 1); 
            triangle.setVec(vec3, 2);

            return in;
        }
        template <typename pType> 
        std::ostream &operator << (std::ostream &out, const Triangle<pType> &triangle) {
            out << "{ " << right.get_vertex(0) << " ;\n " << right.get_vertex(1) << " ;\n " << right.get_vertex(2) << " }\n";
            return out;
        }
    }
}