#include "Atom.h"
#include <iomanip>

Atom::Atom(Point3D coords, char type)
: coords(coords)
, type(type)
{}

Atom::Atom() {}

// ----------------------------------

std::ostream& operator<<(std::ostream& os, Atom &a)
{
    os << std::setw(8) << a.x << std::setw(8) << a.y << std::setw(8) << a.z << std::setw(8) << a.type << '\n';
    return os;
}