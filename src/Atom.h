#pragma once
#include "Point3D.h"
#include <ostream>

struct Atom
{
    union 
    {
        Point3D coords;
        struct {double x,y,z;};
    };

    Atom(Point3D &coords);
    Atom();
};

// ------------------------------------------------

std::ostream& operator<<(std::ostream& os, Atom &a);