#ifndef OCTOTREE_HPP__
#define OCTOTREE_HPP__

#include <iostream>
#include <cmath>
#include <list>
#include "triangle.hpp"
#include "vector.hpp"

namespace Tree {
	const double LittleSpace = 0.1;

	template <typename pType = double>
	class Octotree {

	private:
		using ListIt = typename std::list < GObjects::Triangle <pType> > ::iterator;

		GObjects::Vector<pType> rightBorder_, leftBorder_;	
		Octotree *child_[8] {};
		std::list < GObjects::Triangle<pType> > listOfTriangles_;
		
//-----------------------------------------------------------------------------------------------------

		int whatChapter (GObjects::Vector<pType> &leftBorder, GObjects::Vector<pType> &rightBorder, const GObjects::Triangle<pType> &tr) {
			int chapter[3] {}; // (z,y,x)

			for (int vertexNum = 0; vertexNum < 3; ++vertexNum) {
				GObjects::Vector<pType> tmp = tr.getVec(vertexNum);

				for (int coordNum = 0; coordNum < 3; ++coordNum) {		// 1 is positive
					pType mid = ((leftBorder + rightBorder) / 2.0).getCoord(coordNum);

					if(tmp.getCoord(coordNum) > mid)
						chapter[vertexNum] |= 1 << coordNum;
					else if(tmp.getCoord(coordNum) == mid)
						return -1;
				}

				if(vertexNum){
					if(chapter[vertexNum] != chapter[vertexNum - 1])
						return -1;
				}
			}
			return chapter[0];
		}

//-----------------------------------------------------------------------------------------------------

		void createNewNode (Octotree &curRoot, int chapter) {
			curRoot.child_[chapter] = new Octotree {};
			
			GObjects::Vector<pType> right = curRoot.rightBorder_;
			GObjects::Vector<pType> left = curRoot.leftBorder_;
			GObjects::Vector<pType> mid = (right + left) / 2.0;

			for(size_t i = 0; i < 3; ++i)
			{
				if((chapter >> i) & 1) {
					curRoot.child_[chapter]->leftBorder_.setCoord (i, mid.getCoord(i));
					curRoot.child_[chapter]->rightBorder_.setCoord (i, right.getCoord(i));
				}
				else {
					curRoot.child_[chapter]->leftBorder_.setCoord (i, left.getCoord(i));
					curRoot.child_[chapter]->rightBorder_.setCoord (i, mid.getCoord(i));
				}
			}
		}

//-----------------------------------------------------------------------------------------------------

		void siftTree(Octotree &curRoot) {
			if (curRoot.listOfTriangles_.size() <= 2 || (curRoot.rightBorder_ - curRoot.leftBorder_).squareLength() < LittleSpace)
				return;

			ListIt it = curRoot.listOfTriangles_.begin();
			while (it != curRoot.listOfTriangles_.end()) {
				int chapter = whatChapter (curRoot.leftBorder_, curRoot.rightBorder_, *it);

				if (chapter < 0) {
					++it;
					continue;
				}

				if (!curRoot.child_[chapter])
					createNewNode (curRoot, chapter);

				ListIt tmp = it;

				if (it != curRoot.listOfTriangles_.end())
					tmp = next(it);
				
				curRoot.child_[chapter]->listOfTriangles_.splice(curRoot.child_[chapter]->listOfTriangles_.begin(), curRoot.listOfTriangles_, it);

				it = tmp;
			}

			for (int i = 0; i < 8; ++i) {
				if(!curRoot.child_[i])
					continue;

				siftTree(*(curRoot.child_[i]));
			}
		}

//-----------------------------------------------------------------------------------------------------

	public:

		Octotree (GObjects::Vector<pType> right = 0, GObjects::Vector<pType> left = 0) :
			rightBorder_ (right),
			leftBorder_ (left),
			child_ {} {
		}

//-----------------------------------------------------------------------------------------------------

		~Octotree () {
			for(int i = 0; i < 8; ++i) {
				if(!child_[i])
					continue;

				delete child_[i];
			}
		}

///-----------------------------------------------------------------------------------------------------

		void fillTree(int countTriangles) {			
			pType 	maxInTriangle 	= 0, 
					maxInTree 		= 0;
			GObjects::Triangle<pType> tmp;

			for (int i = 0; i < countTriangles; ++i) {
				std::cin >> tmp;

				// if (!tmp.isTriangle())
				// 	continue;
				// TODO: we must handle degenerate triangles
				
				listOfTriangles_.push_front(tmp);

				maxInTriangle = tmp.getAbsMaxCoord();
				if (maxInTriangle > maxInTree)
					maxInTree = maxInTriangle;
			}
			rightBorder_ = {maxInTree, maxInTree, maxInTree};
			leftBorder_ = - rightBorder_;

			siftTree (*this);

			dumpTree (*this);
		}

//-----------------------------------------------------------------------------------------------------

		void dumpTree (Octotree &curRoot) {
			std::cout << "left = " << curRoot.leftBorder_ << "; right = " << curRoot.rightBorder_ << std::endl;

			for(auto v : curRoot.listOfTriangles_)
				std::cout << v << std::endl;

			std::cout << std::endl;

			for (int i = 0; i < 8; ++i) {
				if(!curRoot.child_[i])
					continue;

				dumpTree (*(curRoot.child_[i]));
			}
		}
	};
}

#endif