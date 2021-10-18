#ifndef OCTOTREE_HPP__
#define OCTOTREE_HPP__

#include <iostream>
#include <cmath>
#include <list>
#include "triangle.hpp"
// #include "vector.hpp"

namespace Tree {
	using pType = double;

	const double LittleSpace = 0.1;

	class Octotree {

	private:
		using ListIt = typename std::list < GObjects::Triangle > ::iterator;

		GObjects::Vector rightBorder_, leftBorder_;	
		Octotree *child_[8] {};
		std::list < GObjects::Triangle > listOfTriangles_;
		
		int whatChapter (GObjects::Vector &leftBorder, GObjects::Vector &rightBorder, const GObjects::Triangle &tr);
		void createNewNode (Octotree &curRoot, int chapter);
		void siftTree(Octotree &curRoot);

//-----------------------------------------------------------------------------------------------------

	public:

		Octotree (GObjects::Vector right = 0, GObjects::Vector left = 0);
		~Octotree ();

		void fillTree(int countTriangles);

		void dumpTree (Octotree &curRoot);
	};
}

#endif
