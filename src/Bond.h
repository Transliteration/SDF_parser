#pragma once
#include <memory>
#include "Atom.h"

// Bond based on weak_ptr
struct Bond
{
    std::weak_ptr<Atom> from, to;
    int type;

    Bond(std::weak_ptr<Atom> from, std::weak_ptr<Atom> to, int type);
};

std::ostream& operator<<(std::ostream& os, Bond &bond);

/* Bond based on atom indexes */
struct iBond
{
    int in1, in2;
    iBond(int in1, int in2);
};