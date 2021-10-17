#include "../include/vector.hpp"

GObjects::pType determinant (GObjects::Vector first, GObjects::Vector second, GObjects::Vector third) {

    return (first.getCoord(0) * second.getCoord(1) * third.getCoord(2) - 
            first.getCoord(0) * second.getCoord(2) * third.getCoord(1) -
            first.getCoord(1) * second.getCoord(0) * third.getCoord(2) +
            first.getCoord(1) * second.getCoord(2) * third.getCoord(0) +
            first.getCoord(2) * second.getCoord(0) * third.getCoord(1) -
            first.getCoord(2) * second.getCoord(1) * third.getCoord(0));
}

GObjects::Vector intersectionPointOfTwoLines (GObjects::Vector begin_1, GObjects::Vector segment_1, GObjects::Vector segment_2, GObjects::Vector segment_3, GObjects::Vector difVec) {
	std::cout << begin_1 << segment_1 << segment_2 << segment_3 << difVec << std::endl;
	
	GObjects::pType det_0 = determinant (segment_1, segment_2, segment_3);

	GObjects::pType detX = determinant (difVec, segment_2, segment_3);

	GObjects::pType x     = detX / det_0;

	GObjects::pType xVec = begin_1.getCoord(0) + x * segment_1.getCoord(0);
	GObjects::pType yVec = begin_1.getCoord(1) + x * segment_1.getCoord(1);
	GObjects::pType zVec = begin_1.getCoord(2) + x * segment_1.getCoord(2);

	return {xVec, yVec, zVec};
}