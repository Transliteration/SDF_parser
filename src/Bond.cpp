#include "Bond.h"
#include <iomanip>
#include <ostream>

/* 
    from - weak_ptr to first  atom
    to   - weak_ptr to second atom
*/
Bond::Bond(std::weak_ptr<Atom> from, std::weak_ptr<Atom> to, int type)
    : from(from)
    , to(to)
    , type(type)
    {}

std::ostream& operator<<(std::ostream& os, Bond &bond)
{
    os << "  From:\t" << *(bond.from.lock().get()) << "    To:\t" << *(bond.to.lock().get());
    return os;
}

/* 
    in1 - index of first  atom
    in2 - index of second atom 
*/
iBond::iBond(int in1, int in2)
: in1(in1)
, in2(in2)
{}