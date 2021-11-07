#include <iostream>
#include "tree.hpp"

void CopyTest ();
void MoveTest ();
void AssignmentTest ();
void DumpAnswer (TreeImpl::Tree &firstTree, TreeImpl::Tree &secondTree,
                const char* method, const char* treeToDump);
void getRand (long long n);

int main () {

    std::cout << "Test" << std::endl;
    CopyTest ();
    MoveTest ();
    AssignmentTest ();
    return 0;

}

namespace {
    
    bool IsEqual (TreeImpl::Node* firstRoot, TreeImpl::Node* secondRoot) {

        if (firstRoot == nullptr && secondRoot == nullptr)
            return true;
        else if (firstRoot == nullptr || secondRoot == nullptr)
            return false;

        if (firstRoot->val_ != secondRoot->val_ || firstRoot->subtreeSize != secondRoot->subtreeSize || firstRoot->color_ != secondRoot->color_)
            return false;

        return (IsEqual (firstRoot->left_, secondRoot->left_) && IsEqual (secondRoot->right_, firstRoot->right_));
    }
}

void CopyTest () {

    TreeImpl::Tree firstTree{};

    for (long long i = 0; i < 50; ++i)
        firstTree.push(i);
    TreeImpl::Tree secondTree{firstTree};

    TreeImpl::Tree thirdTree = secondTree;

    DumpAnswer (firstTree, thirdTree, "COPYCTOR-TEST", "secondtree.dot");
    DumpAnswer (firstTree, thirdTree, "COPYASSIG-TEST", "thirdtree.dot");
}

void MoveTest () {

    TreeImpl::Tree tmpRefRefTree = {};

    for (long long i = 0; i < 50; ++i)
        tmpRefRefTree.push(i);

    TreeImpl::Tree firstTree{std::move (tmpRefRefTree)};

    TreeImpl::Tree thirdTree{};
    for (long long i = 0; i < 50; ++i)
        thirdTree.push(i);

    DumpAnswer (firstTree, thirdTree, "COPYMOVE-TEST", "thirdtree.dot");
}

void AssignmentTest () {

    TreeImpl::Tree firstTree{};
    TreeImpl::Tree secondTree{};

    for (long long i = 0; i < 50; ++i) 
        firstTree.push(i);

    secondTree = firstTree;

    DumpAnswer (firstTree, secondTree, "ASSIGNMENTOP-TEST", "thirdtree.dot");
}

void DumpAnswer (TreeImpl::Tree &firstTree, TreeImpl::Tree &secondTree, 
                const char* method, const char* treeToDump) {

    if (IsEqual (firstTree.getRoot(), secondTree.getRoot())) {

        std::cout << method << " IS PASSED!" << std::endl;
    }
    else {

        std::cout << method << " IS WRONG!" << std::endl;
        firstTree.graphDump ("firsttree.dot");
        secondTree.graphDump (treeToDump);
    }
}

void getRand (long long n) {

    bool *arr = new bool [n] {};

    for (int i = 0, end = n * 100; i < end; ++i) {
        long long rand = std::rand() % n;
        if (arr[rand] == 0) {
            arr[rand] = 1;
            printf("k %lld ", rand + 1);
        }
    }

    for (long long i = 0; i < n; ++i) {
        if (arr[i] == 0) {
            printf("k %lld ", i + 1);
        }
    }

    delete [] arr;
}