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
    template <typename pType> 
	class Plane {
        Vector<pType> nVec_;
        pType d_;

    public:
        Plane (const Vector<pType> &vec, const pType d) :
            nVec_(vec),
            d_(d) {}

		Plane ():
			vec {},
			d_  {} {} //nicely

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
		Triangle (): 
			rVecs_ {} {}

        Triangle (const Vector<pType> &vec1, const Vector<pType> &vec2, const Vector<pType> &vec3) :
            rVecs_{vec1, vec2, vec3} {}

        void setVec (Vector<pType> &vec, int num) { 
            rVecs_[num] = vec;
        }

        Vector<pType> getVec (int num) const { 
            return rVecs_[num];
        }

        pType getAbsMaxCoord () {
            return std::max (
							{(const PType) rVecs_ [0].getAbsMaxCoord (), 
                             (const PType) rVecs_ [1].getAbsMaxCoord (), 
                             (const PType) rVecs_ [2].getAbsMaxCoord ()}
							);
        }   
        
        bool isTriangle () const {
            if (((rVecs_[2] - rVecs_[0]) ^ (rVecs_[1] - rVecs_[0])) == Vector<pType> ({0, 0, 0}))
                return false;
            else    
                return true;
        }

        bool signedDistance (Plane<pType> &plain) {
            pType dists[3]{};

            for (int i = 0; i < 3; ++i)
                dists[i] = rVecs_ [i] * plain.getVec () + plain.getD ();

			long long firstPSide = dists [0];
            return ((( (long long) (dists[0]) & (long long) (dists[1]) & (long long) (dists[2])) | 
					~(((long long) (dists[0]) | (long long) (dists[1]) 
					|  (long long) (dists[2])))) //it was Vlad. Blame it on him :))))))))) 
                                    >> (sizeof(pType) * 8 - 1));
		
		}

		void CalcNormal (Vector<pType> &normalVector) {

			Vector<pType> firstSide 	= rVecs_ [0] - rVecs_ [1];
			Vector<pType> secondSide 	= rVecs_ [0] - rVecs_ [2];

			normalVector = firstSide ^ secondSide;

			normalVector = normalVector / sqrt (normalVector.squareLength ());

		}

		void calcCoefD (Vector<pType> &normalV, pType &ourCoefD) {

			ourCoefD {};

			for (int i = 0; i < 3; ++i) {

				outCoefD += normalV.getCoord (i) * rVecs_ [0].getCoord (i);
				std::cout << ourCoefD << std::endl;

			}

			outCoefD = -outCoefD;

		}

        bool isIntersected (Triangle<pType> &tr) {
			//future exists

			Plane<pType> firstPlane {};
			Plane<pType> secondPlane {};

			Vector<pType> normalVector;
			CalcNormal (normalVector);
			
			Vector<pType> anotherNormalVector;
			tr.CalcNormal (anotherNormalVector);

			pType ourCoefD;
			calcCoefD (normalVector, ourCoefD);

			pType anotherCoefD;
			tr.calcCoefD (anotherNormalVector, anotherCoefD);
			
			if(normalVector == anotherNormalVector) {
				if(coef_d_ == other.coef_d_)
					return false;

				//return intersect_one_plane_triangle_test (other);	//TODO:
			}

			if(tr.signedDistance (firstPlane))
				return false;
			//TODO rename signedDistance
			if(signedDistance (secondPlane))
				return false;
			//fall asleep
			line_t<T> intersection_line {*this, other};

			return intersectTwoTriangle (...);
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
            out << "{ " << triangle.getVec(0) << " ;\n " << triangle.getVec(1) << " ;\n " << triangle.getVec(2) << " }\n";
            return out;
        }

}

#endif