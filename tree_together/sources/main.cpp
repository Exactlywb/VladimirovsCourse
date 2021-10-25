#include "tree.hpp"

int main () {

    TreeImpl::Tree mainTree {};

    mainTree.push (12);

    mainTree.graphDump ("tree.dot");

    return 0;

}
