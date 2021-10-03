#ifndef TRIANGLES_H_
#define TRIANGLES_H_

#include <iostream>

namespace Geometry {

    template <typename coordTypes = double>
    class Point {

        coordTypes x;
        coordTypes y;
        coordTypes z;

    public:
        Point   (coordTypes newX, coordTypes newY, coordTypes newZ):
                x (newX),
                y (newY),
                z (newZ) {}

        coordTypes getX () const {

            return x;

        }

        coordTypes getY () const {

            return y;

        }

        coordTypes getZ () const {

            return z;

        }

        void setX (coordTypes newX) {

            x = newX;

        }

        void setY (coordTypes newY) {

            y = newY;

        }

        void setZ (coordTypes newZ) {

            z = newZ;

        }

    };

    template <typename coordTypes>
    Point<coordTypes> operator + (const Point<coordTypes>& firstPoint, const Point<coordTypes>& secondPoint) {
            
        coordTypes newX = firstPoint.getX () + secondPoint.getX ();
        coordTypes newY = firstPoint.getY () + secondPoint.getY ();
        coordTypes newZ = firstPoint.getZ () + secondPoint.getZ ();

        return Point<coordTypes> (newX, newY, newZ);

    }

    template <typename coordTypes>
    Point<coordTypes> operator - (const Point<coordTypes>& firstPoint, const Point<coordTypes>& secondPoint) {
            
        coordTypes newX = firstPoint.getX () - secondPoint.getX ();
        coordTypes newY = firstPoint.getY () - secondPoint.getY ();
        coordTypes newZ = firstPoint.getZ () - secondPoint.getZ ();

        return Point<coordTypes> (newX, newY, newZ);

    }

    template <typename T>
    std::ostream& operator << (std::ostream &out, const Point<T>& thou) {

        out << "(" << thou.getX () << ", " << thou.getY () << ", " << thou.getZ () << ")";
        return out;

    }

    template <typename T>
    std::istream& operator >> (std::istream &in, Point<T>& thou) {

        T newX;
        T newY;
        T newZ;

        in >> newX >> newY >> newZ;
        
        thou.setX (newX);
        thou.setY (newY);
        thou.setZ (newZ);
        
        return in;

    }

    //TRIANGLE'S AREA
    template <typename pointerType = double> 
    class Triangle {

        Point<pointerType> edge1;
        Point<pointerType> edge2;
        Point<pointerType> edge3;

    public:
        Triangle (Point<pointerType> nEdge1, Point<pointerType> nEdge2, Point<pointerType> nEdge3):
                    edge1 (nEdge1),
                    edge2 (nEdge2),
                    edge3 (nEdge3) {}

        Triangle ():
                    edge1 (Point<pointerType> (0, 0, 0)),
                    edge2 (Point<pointerType> (0, 0, 0)),
                    edge3 (Point<pointerType> (0, 0, 0)) {}

        Point<pointerType> getEdge1 () const {

            return edge1;

        }

        Point<pointerType> getEdge2 () const {

            return edge2;

        }

        Point<pointerType> getEdge3 () const {

            return edge3;

        }

        void setEdge1 (Point<pointerType> nEdge1) {

            edge1 = nEdge1;

        }

        void setEdge2 (Point<pointerType> nEdge2) {

            edge2 = nEdge2;

        }

        void setEdge3 (Point<pointerType> nEdge3) {

            edge3 = nEdge3;

        }

    };

    template <typename pointT>
    std::ostream& operator << (std::ostream &out, const Triangle<pointT>& thou) {

        out << "edge1: " << thou.getEdge1 () << std::endl;
        out << "edge2: " << thou.getEdge2 () << std::endl;
        out << "edge3: " << thou.getEdge3 ();

        return out;

    }

    template <typename pointT>
    std::istream& operator >> (std::istream &in, Geometry::Triangle<pointT>& thou) {

        Point<pointT> nPoint1 (0, 0, 0);
        Point<pointT> nPoint2 (0, 0, 0);
        Point<pointT> nPoint3 (0, 0, 0);

        in >> nPoint1 >> nPoint2 >> nPoint3;
        
        thou.setEdge1 (nPoint1);
        thou.setEdge2 (nPoint2);
        thou.setEdge3 (nPoint3);
        
        return in;

    }

}

#endif
