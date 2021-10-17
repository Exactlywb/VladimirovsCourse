#include "../include/vector.hpp"

GObjects::pType determinant (GObjects::Vector first, GObjects::Vector second, GObjects::Vector third) {

    return (first.getCoord(0) * second.getCoord(1) * third.getCoord(2) - 
            first.getCoord(0) * second.getCoord(2) * third.getCoord(1) -
            first.getCoord(1) * second.getCoord(0) * third.getCoord(2) +
            first.getCoord(1) * second.getCoord(2) * third.getCoord(0) +
            first.getCoord(2) * second.getCoord(0) * third.getCoord(1) -
            first.getCoord(2) * second.getCoord(1) * third.getCoord(0));
}