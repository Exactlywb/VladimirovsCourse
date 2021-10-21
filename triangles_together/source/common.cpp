#include "common.hpp"

int DoubleCmp (const double firstDouble, const double secondDouble) {

    if (std::abs (firstDouble - secondDouble) <= EPSILON)    
        return 0;
    
    if (firstDouble > secondDouble)
        return 1;
    
    return -1;

}
