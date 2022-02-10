#ifndef ERROR_DETECTOR_HPP__
#define ERROR_DETECTOR_HPP__

#include <iostream>

#include "location.hh"

class ErrorDetector final : public std::runtime_error {
    yy::location location_;

public:
    ErrorDetector (const char *err, yy::location loc) : std::runtime_error (err), location_ (loc) {}

    yy::location getLocation () const { return location_; }
};

#endif
