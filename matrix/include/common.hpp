#ifndef COMMON_HPP__
#define COMMON_HPP__

int DblCmp  (const double first, const double second);

int DblCmp (const double first, const double second) {

    static const double eps = 1e-10;
    if (std::abs (first - second) <= eps)
        return 0;
    else if (first > second)
        return 1;
    else
        return -1;

}

#endif
