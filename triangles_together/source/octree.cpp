#include "octree.hpp"

namespace Tree {
    int Octree::whatChapter (GObjects::Vector &leftBorder, GObjects::Vector &rightBorder, const GObjects::Triangle &tr) {
        
        Cmp::cmp DoubleCmp;
        int chapter[3] {}; // (z,y,x)

        for (int vertexNum = 0; vertexNum < 3; ++vertexNum) {
            GObjects::Vector tmp = tr.getVec(vertexNum);

            for (int coordNum = 0; coordNum < 3; ++coordNum) {      // 1 is positive
                pType mid = ((leftBorder + rightBorder) / 2.0).getCoord(coordNum);

                if( DoubleCmp (tmp.getCoord(coordNum), mid) > 0)
                    chapter[vertexNum] |= 1 << coordNum;
                else if(DoubleCmp (tmp.getCoord(coordNum), mid) == 0)
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

    void Octree::createNewNode (Octree &curRoot, int chapter) {

        curRoot.child_[chapter] = new Octree {};
        curRoot.child_[chapter]->parent = &curRoot;
        
        GObjects::Vector right = curRoot.rightBorder_;
        GObjects::Vector left = curRoot.leftBorder_;
        GObjects::Vector mid = (right + left) / 2.0;

        for(int i = 0; i < 3; ++i) {
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

    void Octree::siftTree(Octree &curRoot) {
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

    void Octree::fillTree(int countTriangles) {			
        pType 	maxInTriangle 	= 0, 
                maxInTree 		= 0;
        GObjects::Triangle tmp;

        for (int i = 0; i < countTriangles; ++i) {
            std::cin >> tmp;

            tmp.typeOfDegenerate ();
            tmp.setNumber (i);

            listOfTriangles_.push_front(tmp);

            maxInTriangle = tmp.getAbsMaxCoord();
            if (maxInTriangle > maxInTree)
                maxInTree = maxInTriangle;
        }
        rightBorder_ = {maxInTree, maxInTree, maxInTree};
        leftBorder_ = - rightBorder_;

        siftTree (*this);
    }

//-----------------------------------------------------------------------------------------------------

    void Octree::dumpTree (Octree &curRoot) {
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
}