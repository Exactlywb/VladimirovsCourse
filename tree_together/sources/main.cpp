#include "tree.hpp"

int main () {

    TreeImpl::Tree mainTree {};

    mainTree.push (12);
    mainTree.push (13);
    mainTree.push (9);
    mainTree.push (-2);

    mainTree.graphDump ("tree.dot");

    return 0;

}
