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

}
