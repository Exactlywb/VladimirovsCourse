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

	pType Triangle::getAbsMaxCoord () {
		return std::max ({(const pType) rVecs_ [0].getAbsMaxCoord (), 
						  (const pType) rVecs_ [1].getAbsMaxCoord (), 
						  (const pType) rVecs_ [2].getAbsMaxCoord ()});
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

		long long firstPSide = dists [0];
		return ((( (long long) (dists[0]) & (long long) (dists[1]) & (long long) (dists[2])) | 
				~(((long long) (dists[0]) | (long long) (dists[1]) 
				|  (long long) (dists[2])))) //it was Vlad. Blame it on him :))))))))) 
								>> (sizeof(pType) * 8 - 1));
	
	}

	void Triangle::CalcNormal (Vector &normalVector) {

		Vector firstSide 	= rVecs_ [0] - rVecs_ [1];
		Vector secondSide 	= rVecs_ [0] - rVecs_ [2];

		normalVector = firstSide ^ secondSide;

		normalVector = normalVector / sqrt (normalVector.squareLength ());

	}

	void Triangle::calcCoefD (Vector &normalV, pType &ourCoefD) {

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
		CalcNormal (normalVector);
		
		Vector anotherNormalVector;
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
}