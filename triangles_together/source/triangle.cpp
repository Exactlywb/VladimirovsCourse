#include "../include/triangle.hpp"

namespace GObjects {

	Plane::Plane (const Vector &vec, const pType d) :
            nVec_(vec),
            d_(d) {}

	Plane::Plane ():
		nVec_ {},
		d_  {} {} //nicely

	Vector Plane::getVec () const {
		return nVec_;
	}
	pType Plane::getD () const {
		return d_;
	}

    pType Plane::dist (Vector &vec) const {
        pType dist{};

		for (int index = 0; index < 3; ++index) {

			dist += vec.getCoord(index) * nVec_.getCoord(index);
		}

		return (dist + d_);
    }

    std::ostream &operator << (std::ostream &out, const Plane &plane) {    
        out << plane.getVec(); 
        out << "D = " << plane.getD() << std::endl; 
    
        return out;
    }

	Triangle::Triangle (): 
		rVecs_ {} {}

	Triangle::Triangle (const Vector &vec1, const Vector &vec2, const Vector &vec3) :
		rVecs_{vec1, vec2, vec3} {}

	void Triangle::setVec (Vector &vec, int num) { 
		rVecs_[num] = vec;
	}

	Vector Triangle::getVec (int num) const { 
		return rVecs_[num];
	}

	pType Triangle::getAbsMaxCoord () const {
		return std::max ({rVecs_ [0].getAbsMaxCoord (), 
						  rVecs_ [1].getAbsMaxCoord (), 
						  rVecs_ [2].getAbsMaxCoord ()});
	}   
	
	bool Triangle::isTriangle () const {
		if (((rVecs_[2] - rVecs_[0]) ^ (rVecs_[1] - rVecs_[0])) == Vector ({0, 0, 0}))
			return false;
		else    
			return true;
	}

	bool Triangle::signedDistance (Plane &plain) {
		pType dists[3]{};

		for (int i = 0; i < 3; ++i)
			dists[i] = rVecs_ [i] * plain.getVec () + plain.getD ();

		// long long firstPSide = dists [0];
		return ((( (long long) (dists[0]) & (long long) (dists[1]) & (long long) (dists[2])) | 
				~(((long long) (dists[0]) | (long long) (dists[1]) 
				|  (long long) (dists[2])))) //it was Vlad. Blame it on him :))))))))) 
								>> (sizeof(pType) * 8 - 1));
	
	}

	void Triangle::calcNormal (Vector &normalVector) const {

		Vector firstSide 	= rVecs_ [0] - rVecs_ [1];
		Vector secondSide 	= rVecs_ [0] - rVecs_ [2];

		normalVector = firstSide ^ secondSide;

		normalVector = normalVector / sqrt (normalVector.squareLength ());

	}

	void Triangle::calcCoefD (Vector &normalV, pType &ourCoefD) const {

		ourCoefD = 0;

		for (int i = 0; i < 3; ++i) {

			ourCoefD += normalV.getCoord (i) * rVecs_ [0].getCoord (i);
			std::cout << ourCoefD << std::endl;

		}

		ourCoefD = -ourCoefD;

	}

	bool Triangle::isIntersected (Triangle &tr) {
		//future exists

		Plane firstPlane {};
		Plane secondPlane {};

		Vector normalVector;
		//CalcNormal (normalVector);
		
		Vector anotherNormalVector;
		//tr.CalcNormal (anotherNormalVector);

		// pType ourCoefD;
		//calcCoefD (normalVector, ourCoefD);

		// pType anotherCoefD;
		//tr.calcCoefD (anotherNormalVector, anotherCoefD);
		
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
        return 0;
		// return intersectTwoTriangle (...);
	}

	//##############################################################################
	//                         TRIANGLE-OVERLOAD PART
	//##############################################################################

	std::istream &operator >> (std::istream &in, Triangle &triangle) {

		Vector vec1;
		Vector vec2;
		Vector vec3;

		in >> vec1 >> vec2 >> vec3;

		triangle.setVec(vec1, 0); 
		triangle.setVec(vec2, 1); 
		triangle.setVec(vec3, 2);

		return in;
	}

	std::ostream &operator << (std::ostream &out, const Triangle &triangle) {
		out << "{ " << triangle.getVec(0) << " ;\n " << triangle.getVec(1) << " ;\n " << triangle.getVec(2) << " }\n";
		return out;
	}

    //##############################################################################
    //                         TRIANGLE-3D INTERSECTION (Frolov)
    //##############################################################################
    
    void CountCommonP  (pType firstD, pType secondD, Vector& firstNormalVec, 
                        Vector& secondNormalVec, Vector& commonP) {
    
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
    
    void ProjectEdges  (pType projection [3], const Triangle& tr, 
                        const Vector& leadVec, const Vector& commonP) {
    
        for (int i = 0; i < 3; i++) {
        
            projection [i] = (tr.getVec (i) - commonP) * leadVec;
        
        }
    
    }

    bool IsIntersectedTIntervals (pType firstTParams [2], pType secondTParams [2]) {
    
        if (firstTParams [0] > firstTParams [1])
            std::swap (firstTParams [0], firstTParams [1]);

        for (int i = 0; i < 2; i++) {
        
            if (firstTParams [0] <= secondTParams [i] && firstTParams [1] >= secondTParams [i])
                return 1;

        }

        return 0;
    
    }

    pType CalcDist (const Vector& normalV, const pType dCoef, const Vector& point) {
    
        pType dist =    normalV.getCoord (0) * point.getCoord (0) + 
                        normalV.getCoord (1) * point.getCoord (1) +
                        normalV.getCoord (2) * point.getCoord (2) + dCoef;

        return dist; 
    
    }

    void CalcTParams (pType tParams [2], const pType projection [3], 
                      const Vector& normalV, const pType dCoef, const Triangle& tr) {
    
        pType distThirdVert = CalcDist (normalV, dCoef, tr.getVec (2));

        for (int i = 0; i < 2; i++) {
            
            pType verticeDist = CalcDist (normalV, dCoef, tr.getVec (i));
            pType distFrac = verticeDist / (verticeDist - distThirdVert);

            tParams [i] = projection [i] + (projection [2] - projection [i]) * distFrac;
        
        }
    
    }

    bool Intersect3DTriangles (const Triangle& tr1, const Triangle& tr2) {
        //Normal vector for the first plane
        Vector firstNormalVec;
        tr1.calcNormal (firstNormalVec);
        
        //Coef D for the first plane
        pType firstD = 0;
        tr1.calcCoefD (firstNormalVec, firstD);

        //Normal vector for the second plane
        Vector secondNormalVec;
        tr2.calcNormal (secondNormalVec);
        
        //Coef D for the second plane
        pType secondD = 0;
        tr2.calcCoefD (secondNormalVec, secondD);

        //leading vector for the common lane
        Vector leadVec = firstNormalVec ^ secondNormalVec;
        
        Vector commonP;
        CountCommonP (firstD, secondD, firstNormalVec, secondNormalVec, commonP);

        //result: now we have common point and direction vector for the common lane
        std::cout << "Common point: " << commonP << std::endl;
        std::cout << "Direction: " << leadVec << std::endl;

        //project triangle's vertices
        pType firstProj [3] = {};
        ProjectEdges (firstProj, tr1, leadVec, commonP);
        //std::cout << "proj " << firstProj [0] << " " << firstProj [1] << " " << firstProj [2] << std::endl;
        pType secondProj [3] = {};
        ProjectEdges (secondProj, tr2, leadVec, commonP);

        //now let's compute t_{0,i} params. Better watch GCT 578 page 
        pType firstTParams [2] = {};
        CalcTParams (firstTParams, firstProj, secondNormalVec, secondD, tr1);
        
        pType secondTParams [2] = {};
        CalcTParams (secondTParams, secondProj, firstNormalVec, firstD, tr2);

        for (size_t i = 0; i < 2; i++) {
        
            std::cout << "firstT: "  << firstTParams  [i] << std::endl;
            std::cout << "secondT: " << secondTParams [i] << std::endl;
        
        }

        return IsIntersectedTIntervals (firstTParams, secondTParams);
    }

		//##############################################################################
    //                         TRIANGLE-2D INTERSECTION (Belov)
    //##############################################################################

    bool Triangle::intersectOnePlaneTriangle (Triangle &tr) {
        
        for (int firstTriangleCounter = 0; firstTriangleCounter < 3; ++firstTriangleCounter) {

            for (int secondTriangleCounter = 0; secondTriangleCounter < 3; ++secondTriangleCounter) {

                if (intersectSegments (rVecs_[firstTriangleCounter], -rVecs_[firstTriangleCounter] + rVecs_[(firstTriangleCounter + 1) % 3], 
                    tr.rVecs_[secondTriangleCounter], -tr.rVecs_[secondTriangleCounter] + tr.rVecs_[(secondTriangleCounter + 1) % 3]))
                    return true;
            }
        }
        return false;
    }

    bool intersectSegments (Vector begin_1, Vector segment_1, Vector begin_2, Vector segment_2) {
        
        Vector cross  = segment_1 ^ segment_2;
        Vector difVec = begin_2 - begin_1;

        // std::cout << cross << std::endl;

        std::cout << "Here!\n";

        if (cross == Vector {}) {

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
        // pType detY = determinant (segment_1, difVec, cross);
        // std::cout << "detY = " << detY << std::endl;
        // pType detZ = determinant (segment_1, segment_2, difVec);
        // std::cout << "detZ = " << detZ << std::endl;

        pType x     = detX / det_0;
        // pType y     = detY / det_0;
        // pType z     = detZ / det_0; 

        // std::cout << "X = " << x << std::endl;
        // std::cout << "Y = " << y << std::endl;
        // std::cout << "Z = " << z << std::endl;

        pType xVec = begin_1.getCoord(0) + x * segment_1.getCoord(0);
        pType yVec = begin_1.getCoord(1) + x * segment_1.getCoord(1);
        // pType zVec = begin_1.getCoord(2) + x * segment_1.getCoord(2); 

        Vector interPoint = {xVec, yVec, 0};

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

