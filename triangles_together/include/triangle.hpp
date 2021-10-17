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
			nVec_ {},
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
							{(const pType) rVecs_ [0].getAbsMaxCoord (), 
                             (const pType) rVecs_ [1].getAbsMaxCoord (), 
                             (const pType) rVecs_ [2].getAbsMaxCoord ()}
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

			ourCoefD = 0;

			for (int i = 0; i < 3; ++i) {

				ourCoefD += normalV.getCoord (i) * rVecs_ [0].getCoord (i);
			}

			ourCoefD = -ourCoefD;

		}

        bool intersectOnePlaneTriangle (Triangle<pType> &tr) {
            
            std::cout << " wefwfwef\n";
            for (int firstTriangleCounter = 0; firstTriangleCounter < 3; ++firstTriangleCounter) {

                for (int secondTriangleCounter = 0; secondTriangleCounter < 3; ++secondTriangleCounter) {

                    if (intersectSegments (rVecs_[firstTriangleCounter], -rVecs_[firstTriangleCounter] + rVecs_[(firstTriangleCounter + 1) % 3], 
                        tr.rVecs_[secondTriangleCounter], -tr.rVecs_[secondTriangleCounter] + tr.rVecs_[(secondTriangleCounter + 1) % 3]))
                        return true;
                }
            }
            return false;
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
				if(ourCoefD != anotherCoefD)
					return false;

				return intersectOnePlaneTriangle (tr);	//TODO:
			}

			if(tr.signedDistance (firstPlane))
				return false;
			//TODO rename signedDistance
			if(signedDistance (secondPlane))
				return false;
			//fall asleep

			// return intersectTwoTriangle (...);
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

    template <typename pType>
    bool intersectSegments (Vector<pType> begin_1, Vector<pType> segment_1, Vector<pType> begin_2, Vector<pType> segment_2) {
        
        

        Vector<pType> cross  = segment_1 ^ segment_2;
        Vector<pType> difVec = begin_2 - begin_1;

        // std::cout << cross << std::endl;

        std::cout << "Here!\n";

        if (cross == Vector<pType> {}) {

            std::cout << "Cross == 0" << std::endl;
            char counter = 0;

            for (int coordNum = 0; coordNum < 3; ++coordNum) {
        
                pType first  = begin_1.getCoord(coordNum);
                pType second = begin_1.getCoord(coordNum) + segment_1.getCoord(coordNum);

                if (std::fabs(std::min (first, second) - begin_2.getCoord(coordNum)) < __DBL_EPSILON__ && 
                    std::fabs(std::max (first, second) - begin_2.getCoord(coordNum)) < __DBL_EPSILON__)
                        ++counter;
            }

            if (counter == 3) return true;

            counter = 0;

            for (int coordNum = 0; coordNum < 3; ++coordNum) {
        
                pType first  = begin_1.getCoord(coordNum);
                pType second = begin_1.getCoord(coordNum) + segment_1.getCoord(coordNum);

                if (std::min (first, second) <= begin_2.getCoord(coordNum) + segment_2.getCoord(coordNum) && 
                    std::max (first, second) >= begin_2.getCoord(coordNum) + segment_2.getCoord(coordNum))
                        ++counter;
            }

            if (counter == 3) return true;
            return false;
        }

        pType det_0 = determinant (segment_1, segment_2, cross);

        // std::cout << "det_0 = " << det_0 << std::endl;

        pType detX = determinant (difVec, segment_2, cross);
        // std::cout << "detX = " << detX << std::endl;
        pType detY = determinant (segment_1, difVec, cross);
        // std::cout << "detY = " << detY << std::endl;
        pType detZ = determinant (segment_1, segment_2, difVec);
        // std::cout << "detZ = " << detZ << std::endl;

        pType x     = detX / det_0;
        pType y     = detY / det_0;
        pType z     = detZ / det_0; 

        // std::cout << "X = " << x << std::endl;
        // std::cout << "Y = " << y << std::endl;
        // std::cout << "Z = " << z << std::endl;

        pType xVec = begin_1.getCoord(0) + x * segment_1.getCoord(0);
        pType yVec = begin_1.getCoord(1) + x * segment_1.getCoord(1);
        pType zVec = begin_1.getCoord(2) + x * segment_1.getCoord(2); 

        Vector<pType> interPoint = {xVec, yVec, 0};

        std::cout << "Interpoint " << interPoint << std::endl;

        // std::cout << "xVec = " << xVec << std::endl;
        // std::cout << "yVec = " << yVec << std::endl;
        // std::cout << "zVec = " << zVec << std::endl;

        char counter = 0;

        // std::cout << "BEGIN_1 = " << begin_1 << " END_1 = " << begin_1 + segment_1 << " ; BEGIN_2 = " << begin_2 << " END_2 = " << begin_2 + segment_2 << std::endl; 

        for (int coordNum = 0; coordNum < 3; ++coordNum) {
        
            pType first  = begin_1.getCoord(coordNum);
            pType second = begin_1.getCoord(coordNum) + segment_1.getCoord(coordNum);

            if (std::min (first, second) <= interPoint.getCoord(coordNum)  && 
                std::max (first, second) >= interPoint.getCoord(coordNum))
                    ++counter;
        }
        // std::cout << "Counter " << (int)counter << std::endl;

        for (int coordNum = 0; coordNum < 3; ++coordNum) {
        
            pType first  = begin_2.getCoord(coordNum);
            pType second = begin_2.getCoord(coordNum) + segment_2.getCoord(coordNum);

            if (std::min (first, second) <= interPoint.getCoord(coordNum)  && 
                std::max (first, second) >= interPoint.getCoord(coordNum))
                    ++counter;
        }

        // std::cout << "Counter " << (int)counter << std::endl;

        if (counter == 6) return true;       
        return false;
    }

}

#endif