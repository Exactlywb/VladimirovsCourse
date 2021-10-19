#include "triangle.hpp"

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
        rVecs_ {} {

        typeOfDegeneration_ = 2; //Point
        
    }

    Triangle::Triangle (const Vector &vec1, const Vector &vec2, const Vector &vec3) :
        rVecs_{vec1, vec2, vec3} {
        
        typeOfDegenerate ();    //Set type of triangle degenerate

    }

    void Triangle::setVec (Vector &vec, int num) { 
        rVecs_[num] = vec;
        typeOfDegenerate();
    }

    Vector Triangle::getVec (int num) const { 
        return rVecs_[num];
    }

    int Triangle::getNumber () const{
        return number_;
    }

    void Triangle::setNumber (const int number) {
        number_ = number;
    }

    pType Triangle::getAbsMaxCoord () const {
        return std::max   ({rVecs_ [0].getAbsMaxCoord (), 
                            rVecs_ [1].getAbsMaxCoord (), 
                            rVecs_ [2].getAbsMaxCoord ()});
    }

    pType Triangle::getAbsMinCoord () const {
        return std::min   ({rVecs_ [0].getAbsMinCoord (), 
                            rVecs_ [1].getAbsMinCoord (), 
                            rVecs_ [2].getAbsMinCoord ()});
    }   

    void Triangle::typeOfDegenerate () {

        Vector firstV = rVecs_ [0];

        if ((((rVecs_[2] - firstV) ^ (rVecs_[1] - firstV))) == Vector (0, 0, 0)) {
            if(DoubleCmp(getAbsMaxCoord (), getAbsMinCoord ()) == 0){
                typeOfDegeneration_ = (1 << 1);        //point
                return;
            }
            else{
                typeOfDegeneration_ = (1 << 2);         //segment
                return;
            }
        }
        typeOfDegeneration_ = 1;    //none degenerated
    }

    char Triangle::getDegenerationType () const {
        return typeOfDegeneration_;
    }

    bool Triangle::signedDistance (const Plane &plain) const {

        pType dists[3]{};

        for (int i = 0; i < 3; ++i)
            dists[i] = rVecs_ [i] * plain.getVec () + plain.getD ();

        if (dists[0] * dists[1] > 0)
            if (dists[0] * dists[2] > 0)
                return true;

        return false;	
    }

    void Triangle::calcNormal (Vector &normalVector) const {
        
        Vector firstV = rVecs_ [0];

        Vector firstSide 	= firstV - rVecs_ [1];
        Vector secondSide 	= firstV - rVecs_ [2];

        normalVector = firstSide ^ secondSide;

        normalVector = normalVector / sqrt (normalVector.squareLength ());

    }

    void Triangle::calcCoefD (Vector &normalV, pType &ourCoefD) const {

        ourCoefD = 0;

        for (int i = 0; i < 3; ++i)
            ourCoefD += normalV.getCoord (i) * rVecs_ [0].getCoord (i);

        ourCoefD = -ourCoefD;

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
        if (secondTParams [0] > secondTParams [1])
            std::swap (secondTParams [0], secondTParams [1]);

        for (int i = 0; i < 2; i++) {
            
            if (firstTParams [0] <= secondTParams [i] && firstTParams [1] >= secondTParams [i])
                return 1;
            if (secondTParams[0] <= firstTParams[i] && secondTParams[1] >= firstTParams[i])
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
                            
        int outlaw;

        if (CalcDist (normalV, dCoef, tr.getVec (0)) * CalcDist (normalV, dCoef, tr.getVec (1)) > 0) 
            outlaw = 2;
        else if(CalcDist (normalV, dCoef, tr.getVec (0)) * CalcDist (normalV, dCoef, tr.getVec (2)) > 0)
            outlaw = 1;
        else outlaw = 0;

        pType distThirdVert = CalcDist (normalV, dCoef, tr.getVec (outlaw));

        int curTParamInd = 0;
        for (int i = 0; i < 3; i++) {
            if(i == outlaw)
                continue;
            pType vertexDist = CalcDist (normalV, dCoef, tr.getVec (i));
            pType distFrac = vertexDist / (vertexDist - distThirdVert);

            tParams [curTParamInd++] = projection [i] + (projection [outlaw] - projection [i]) * distFrac;
        }
    }

    //TODO reconstruct file
    static bool HandleDegeneratedCases (const Triangle& tr1, const Triangle& tr2, const char degFlag) {

        //There is a reason to ask why flag :   not degenerated (tr)    = 1 (0b1)
        //                                      point                   = 2 (0b10)
        //                                      segment                 = 4 (0b100)
        //
        //To clarify this let's build the table for the degFlag = tr1.getDegenerationType () | tr2.getDegenerationType ();
        //                  |   (0b1) tr    |   (0b10) point    | (0b100) segment
        // (0b1)   tr       |      0b10     |       0b11        |       0b101
        // (0b10)  point    |      0b11     |       0b100       |       0b110
        // (0b100) segment  |      0b101    |       0b110       |       0b1000
        //
        //As we can see, there's the only one value for every combination (up to permutation). 

        switch (degFlag) {

            
            case 0b11: {

                if (tr1.getDegenerationType () == 0b10) //tr1 is a point
                    return IntersectDegenerates (tr2, tr1.getVec (0));
                else
                    return IntersectDegenerates (tr1, tr2.getVec (0));
                
            }
            case 0b100:
                return IntersectDegenerates (tr1.getVec (0), tr2.getVec (0));
            case 0b101: {

                if (tr1.getDegenerationType () == 0b100) //tr1 is a segment
                    return IntersectDegenerates (tr2, Segment (tr1));
                else 
                    return IntersectDegenerates (tr1, Segment (tr2));

            }
            case 0b110: {

                if (tr1.getDegenerationType () == 0b10) //tr1 is a point 
                {
                    Segment tr2seg(tr2);
                
                    return IntersectDegenerates (tr2seg, tr1.getVec (0));
                }
                else
                {
                    Segment tr1seg(tr1);
                    return IntersectDegenerates (tr1seg, tr2.getVec (0));
                }

            }
            case 0b1000: {

                Segment tr1Seg (tr1);
                Segment tr2Seg (tr2);

                return IntersectDegenerates (Segment (tr1), Segment (tr2));

            }
            default:
                std::cout << "Unexpected bitmask in function " << (int) degFlag << " " << __func__ << std::endl;
                return false;

        }

    }

    bool Intersect3DTriangles (const Triangle& tr1, const Triangle& tr2) {
        
        //Handling for the degenerated triangles

        char degFlag = tr1.getDegenerationType () + tr2.getDegenerationType ();
        if(degFlag != (1 << 1)) 
            return HandleDegeneratedCases (tr1, tr2, degFlag);

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

        if ((firstNormalVec ^ secondNormalVec) == 0) {
            if (DoubleCmp(firstD, secondD))
                return false;
            
            return Intersect2DTriangles(tr1, tr2);
        }

        if (tr1.signedDistance ({secondNormalVec, secondD}))
            return false;
    
        if (tr2.signedDistance ({firstNormalVec, firstD})) 
            return false;
        

        //leading vector for the common lane
        Vector leadVec = firstNormalVec ^ secondNormalVec;
        
        Vector commonP;
        CountCommonP (firstD, secondD, firstNormalVec, secondNormalVec, commonP);
        //result: now we have common point and direction vector for the common lane

        //project triangle's vertices
        pType firstProj [3] = {};
        ProjectEdges (firstProj, tr1, leadVec, commonP);
        pType secondProj [3] = {};
        ProjectEdges (secondProj, tr2, leadVec, commonP);

        //now let's compute t_{0,i} params. Better watch GCT 578 page 
        pType firstTParams [2] = {};
        CalcTParams (firstTParams, firstProj, secondNormalVec, secondD, tr1);
        
        pType secondTParams [2] = {};
        CalcTParams (secondTParams, secondProj, firstNormalVec, firstD, tr2);

        return IsIntersectedTIntervals (firstTParams, secondTParams);
        
    }

    //##############################################################################
    //                         TRIANGLE-2D INTERSECTION (Belov)
    //##############################################################################
    
    static bool CheckPointInSegment (const Vector& beginPVec, const Vector& directVec) {
        
        double vecLen = beginPVec.squareLength ();

        double directLen = directVec.squareLength ();
        pType dir = ((beginPVec * directVec) / (std::sqrt(vecLen) * std::sqrt(directLen)));

        if (DoubleCmp (dir, 1.0) == 0 && DoubleCmp (directLen, vecLen) >= 0)
            return true;
    
        return false;

    }

    bool Triangle::pointInTriangle (const Vector &point) const{
        Vector side = rVecs_[2] - rVecs_[1];

        Vector beginPVec = point - rVecs_[0];

        if(beginPVec == 0)
            return true;

        Vector cross = side ^ beginPVec;

        Vector vectorPsOfTwoLines = IntersectionPointOfTwoLines (rVecs_[1], side, beginPVec, cross, rVecs_[0] - rVecs_[1]) - rVecs_[0];
        
        return CheckPointInSegment (beginPVec, vectorPsOfTwoLines);

    } 

    bool Intersect2DTriangles (const Triangle &tr1, const Triangle &tr2) {	
        if(tr2.pointInTriangle(tr1.getVec(0)) || tr1.pointInTriangle(tr2.getVec(0)))
            return true;

        for (int firstTriangleCounter = 0; firstTriangleCounter < 3; ++firstTriangleCounter) {

            for (int secondTriangleCounter = 0; secondTriangleCounter < 3; ++secondTriangleCounter) {
                
                Vector tr1CurVec = tr1.getVec (firstTriangleCounter);
                Vector tr2CurVec = tr2.getVec (secondTriangleCounter);
                if (IntersectSegments  (tr1CurVec, -tr1CurVec + tr1.getVec((firstTriangleCounter  + 1) % 3), 
                                        tr2CurVec, -tr2CurVec + tr2.getVec((secondTriangleCounter + 1) % 3)))
                    return true;
            }
        }
        return false;
    }

    Vector IntersectionPointOfTwoLines (const Vector &begin_1, const Vector &segment_1, const Vector &segment_2, const Vector &segment_3, const Vector &difVec) {

        pType det_0 = determinant (segment_1, segment_2, segment_3);

        pType detX = determinant (difVec, segment_2, segment_3);

        pType x     = detX / det_0;

        pType xVec = begin_1.getCoord(0) + x * segment_1.getCoord(0);
        pType yVec = begin_1.getCoord(1) + x * segment_1.getCoord(1);
        pType zVec = begin_1.getCoord(2) + x * segment_1.getCoord(2);

        return {xVec, yVec, zVec};
    }

    static void CheckCurCoords (const pType first, const pType second, const pType toCmp, char& counter) {
        
        pType minCoord = std::min (first, second);
        pType maxCoord = std::max (first, second);

        if (DoubleCmp (minCoord, toCmp) <= 0 &&
            DoubleCmp (maxCoord, toCmp) >= 0)
            ++counter;

    }

    bool IntersectSegments (const Vector& begin_1, const Vector& segment_1, const Vector& begin_2, const Vector& segment_2) { 

        Vector cross  = segment_1 ^ segment_2;
        Vector difVec = begin_2 - begin_1;

        if (cross == GetZeroVector ()) {

            if (!(((begin_2 - begin_1) ^ segment_1) == GetZeroVector ()))
                return false;

            char counter = 0;

            for (int coordNum = 0; coordNum < 3; ++coordNum) {
        
                pType curBeginCoord = begin_1.getCoord (coordNum);
                CheckCurCoords (curBeginCoord, 
                                curBeginCoord + segment_1.getCoord (coordNum),
                                begin_2.getCoord (coordNum),
                                counter);

            }

            if (counter == 3) return true;

            counter = 0;

            for (int coordNum = 0; coordNum < 3; ++coordNum) {

                pType curBeginCoord = begin_1.getCoord (coordNum);
                CheckCurCoords (curBeginCoord,
                                curBeginCoord + segment_1.getCoord (coordNum),
                                begin_2.getCoord (coordNum) + segment_2.getCoord (coordNum),
                                counter);

            }

            if (counter == 3) return true;
            return false;
        }

        Vector interPoint = IntersectionPointOfTwoLines(begin_1, segment_1, segment_2, cross, difVec);

        char counter = 0;

        for (int coordNum = 0; coordNum < 3; ++coordNum) {
        
            pType curBeginCoord = begin_1.getCoord (coordNum);
            CheckCurCoords (curBeginCoord,
                            curBeginCoord + segment_1.getCoord (coordNum),
                            interPoint.getCoord (coordNum),
                            counter);

        }

        for (int coordNum = 0; coordNum < 3; ++coordNum) {

            pType curBeginCoord = begin_2.getCoord (coordNum);
            CheckCurCoords (curBeginCoord,
                            curBeginCoord + segment_2.getCoord (coordNum),
                            interPoint.getCoord (coordNum),
                            counter);

        }

        if (counter == 6) return true;       
        return false;
    }

    //##############################################################################
    //                          DEGENERATE INTERSECTION
    //##############################################################################
    bool IntersectDegenerates (const Triangle &tr, const Vector &point) {
        return tr.pointInTriangle(point);
    }

    bool IntersectDegenerates (const Triangle &tr, const Segment &segment) {

        Vector norm{};
        pType coefD = 0;
        tr.calcNormal (norm);
        tr.calcCoefD (norm, coefD);

        Vector end = segment.begin_ + segment.direct_;
        pType beginDist = CalcDist (norm, coefD, segment.begin_);
        pType endDist = CalcDist (norm, coefD, end);

        if (DoubleCmp (beginDist, 0) == 0) {
            if (tr.pointInTriangle (segment.begin_)) //TODO remove from class Triangle
                return true;

        }

        if (DoubleCmp(endDist, 0) == 0) {
            if (tr.pointInTriangle (end))
                return true;
        }

        if (DoubleCmp (beginDist, 0) == 0 && DoubleCmp(endDist, 0) == 0) {
            for (int i = 0; i < 3; ++i)
                if(IntersectSegments (segment.begin_, segment.direct_, tr.getVec(i), tr.getVec((i + 1) % 3) - tr.getVec(i)))
                    return true;
            
            return false;
        }

        if (beginDist * endDist > 0)
            return false;

        pType param = (-coefD - segment.begin_ * norm) / (norm * segment.direct_);
        
        Vector intersectionPoint {};
        for (int i = 0; i < 3; ++i) {
            intersectionPoint.setCoord(i, segment.begin_.getCoord(i) + param * segment.direct_.getCoord(i));
        }

        return tr.pointInTriangle (intersectionPoint);
    }

    bool IntersectDegenerates (const Segment &segment1, const Segment &segment2) {
        
        Vector firstBeginVec  = segment1.begin_;
        Vector secondBeginVec = segment2.begin_;

        Vector connectingVec  = secondBeginVec - firstBeginVec;

        double mixedProduct   = firstBeginVec * (secondBeginVec ^ connectingVec);

        if (DoubleCmp (mixedProduct, 0.0) == 0)
        {

            return IntersectSegments    (segment1.begin_, segment1.direct_, 
                                         segment2.begin_, segment2.direct_); //TODO maybe it's better to rewrite InsersectSegments
        }

        return false;

    }

    bool IntersectDegenerates (const Segment &segment, const Vector &point) {

        Vector beginVec     = segment.begin_;
        Vector directVec    = segment.direct_;

        Vector pBeginVec    = point - beginVec;

        Vector crossProduct = pBeginVec ^ directVec;
        if (crossProduct == GetZeroVector ())
            return CheckPointInSegment (pBeginVec, directVec);

        return false;

    }

    bool IntersectDegenerates (const Vector &point1, const Vector &point2) {
        if (point1 == point2)
            return true;
        return false;
    }

    const Vector GetZeroVector () {

        static Vector zeroVector {0, 0, 0};
        return zeroVector;

    }

}