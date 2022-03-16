#pragma once
#include <memory>
#include "Atom.h"

struct Bond
{
    std::weak_ptr<Atom> from, to;
    int type;

    Bond(std::weak_ptr<Atom> from, std::weak_ptr<Atom> to, int type);
};

std::ostream& operator<<(std::ostream& os, Bond &bond);