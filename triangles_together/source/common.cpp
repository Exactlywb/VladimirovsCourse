#include "../include/common.hpp"

int DoubleCmp (const double firstDouble, const double secondDouble) {

    if (std::abs (firstDouble - secondDouble) < __DBL_EPSILON__) 
        return 0;
    else if (firstDouble > secondDouble)
        return 1;
    else 
        return -1;

}
