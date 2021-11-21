#include "common.hpp"

int DblCmp (const double first, const double second) {

    static const double eps = 1e-14;
    if (std::abs (first - second) <= eps)
        return 0;
    else if (first > second)
        return 1;
    else
        return -1;

}
