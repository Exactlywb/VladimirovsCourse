#ifndef LOCATOR_HPP__
#define LOCATOR_HPP__

#include "location.hh"

class NodeLocator {
    yy::location location_;

public:
    NodeLocator (yy::location loc) : location_ (loc) {}
    NodeLocator () = default;

    yy::location getLocation () const { return location_; }

    virtual ~NodeLocator () = 0;
};

inline NodeLocator::~NodeLocator () = default;

#endif
