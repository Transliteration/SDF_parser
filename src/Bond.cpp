#include "Bond.h"
#include <iomanip>
#include <ostream>

Bond::Bond(std::weak_ptr<Atom> from, std::weak_ptr<Atom> to, int type)
    : from(from)
    , to(to)
    , type(type)
    {}

std::ostream& operator<<(std::ostream& os, Bond &bond)
{
    os << '\t' << *(bond.from.lock().get()) << "\t" << *(bond.to.lock().get());
    return os;
}