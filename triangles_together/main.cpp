#include <iostream>
#include "include/octotree.hpp"

int main()
{
	int countTriangles;
	std::cin >> countTriangles;

	Tree::Octotree <double> tree;

	tree.fillTree(countTriangles);

	return 0;
}