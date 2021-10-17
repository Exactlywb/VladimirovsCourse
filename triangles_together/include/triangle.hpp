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

		void calcNormal (Vector<pType> &normalVector) const {

			Vector<pType> firstSide 	= rVecs_ [0] - rVecs_ [1];
			Vector<pType> secondSide 	= rVecs_ [0] - rVecs_ [2];

			normalVector = firstSide ^ secondSide;

			normalVector = normalVector / sqrt (normalVector.squareLength ());

		}

		void calcCoefD (Vector<pType> &normalV, pType &ourCoefD) const {

			ourCoefD = 0;

			for (int i = 0; i < 3; ++i) {

				ourCoefD += normalV.getCoord (i) * rVecs_ [0].getCoord (i);
				//std::cout << ourCoefD << std::endl;

			}

			ourCoefD = -ourCoefD;

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
			
			// if(normalVector == anotherNormalVector) {
			// 	if(d_ == other.d_)
			// 		return false;

				//return intersect_one_plane_triangle_test (other);	//TODO:
			// }

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

        
        //##############################################################################
        //                         TRIANGLE-3D INTERSECTION (Frolov)
        //##############################################################################
         
        template<typename pType>
        void CountCommonP  (pType firstD, pType secondD, Vector<pType>& firstNormalVec, 
                            Vector<pType>& secondNormalVec, Vector<pType>& commonP) {
        
            //here we calc pointer(P) on common lane
            //\vec{P} = a \cdot \vec{n_1} + b \cdot \vec{n_2}
            pType nScalarProduct    = firstNormalVec * secondNormalVec;
            pType n1SqLength        = firstNormalVec.squareLength ();
            pType n2SqLength        = secondNormalVec.squareLength ();

            pType commonDenominator = nScalarProduct * nScalarProduct - n1SqLength * n2SqLength;

            pType aCoef = (secondD * nScalarProduct - firstD  * n2SqLength) / commonDenominator;
            pType bCoef = (firstD  * nScalarProduct - secondD * n1SqLength) / commonDenominator;
            
            commonP = aCoef * firstNormalVec + bCoef * secondNormalVec;
        
        }
        
        template<typename pType>
        void ProjectEdges  (pType projection [3], const Triangle<pType>& tr, 
                            const Vector<pType>& leadVec, const Vector<pType>& commonP) {
        
            for (int i = 0; i < 3; i++) {
            
                projection [i] = (tr.getVec (i) - commonP) * leadVec;
            
            }
        
        }

        template<typename pType>
        pType CalcDist (const Vector<pType>& normalV, const pType dCoef, const Vector<pType>& point) {
        
            pType dist = std::abs  (normalV.getCoord (0) * point.getCoord (0) + 
                                    normalV.getCoord (1) * point.getCoord (1) +
                                    normalV.getCoord (2) * point.getCoord (2) + dCoef) / std::sqrt(normalV.squareLength ());

            return dist; 
        
        }

        template<typename pType>
        void CalcTParams (pType tParams [2], const pType projection [3], 
                          const Vector<pType> normalV, const pType dCoef, const Triangle<pType>& tr) {
        
            pType distThirdVert = CalcDist (normalV, dCoef, tr.getVec (2));
            for (int i = 0; i < 2; i++) {
                
                pType verticeDist = CalcDist (normalV, dCoef, tr.getVec (i));
                pType distFrac = verticeDist / (verticeDist - distThirdVert);

                tParams [i] = projection [i] + (projection [2] - projection [i]) * distFrac;
                std::cout << "here " << distThirdVert << std::endl;
            
            }
        
        }

        template<typename pType>
        bool Intersect3DTriangles (const Triangle<pType>& tr1, const Triangle<pType>& tr2) {
            //Normal vector for the first plane
            Vector<pType> firstNormalVec;
            tr1.calcNormal (firstNormalVec);
            
            //Coef D for the first plane
            pType firstD = 0;
            tr1.calcCoefD (firstNormalVec, firstD);

            //Normal vector for the second plane
            Vector<pType> secondNormalVec;
            tr2.calcNormal (secondNormalVec);
            
            //Coef D for the second plane
            pType secondD = 0;
            tr2.calcCoefD (secondNormalVec, secondD);

            //leading vector for the common lane
            Vector<pType> leadVec = firstNormalVec ^ secondNormalVec;
            
            Vector<pType> commonP;
            CountCommonP (firstD, secondD, firstNormalVec, secondNormalVec, commonP);

            //result: now we have common point and direction vector for the common lane
            std::cout << "Common point: " << commonP << std::endl;
            std::cout << "Direction: " << leadVec << std::endl;

            //project triangle's vertices
            pType firstProj [3] = {};
            ProjectEdges (firstProj, tr1, leadVec, commonP);
            std::cout << "proj " << firstProj [0] << " " << firstProj [1] << " " << firstProj [2] << std::endl;
            pType secondProj [3] = {};
            ProjectEdges (secondProj, tr2, leadVec, commonP);

            //now let's compute t_{0,i} params. Better watch GCT 578 page 
            pType firstTParams [2] = {};
            CalcTParams (firstTParams, firstProj, firstNormalVec, firstD, tr1);
            
            pType secondTParams [2] = {};
            CalcTParams (secondTParams, secondProj, secondNormalVec, secondD, tr2);

            for (size_t i = 0; i < 2; i++) {
            
                std::cout << "firstT: "  << firstTParams  [i] << std::endl;
                std::cout << "secondT: " << secondTParams [i] << std::endl;
            
            }

            return 0;
        }

}

#endif
