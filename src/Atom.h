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
    char type;

    Atom(Point3D coords, char type = ' ');
    Atom();
};

// ------------------------------------------------

std::ostream& operator<<(std::ostream& os, Atom &a);