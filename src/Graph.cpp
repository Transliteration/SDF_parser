#include "Graph.h"


Graph::Graph(size_t atom_count, std::vector<Point3D> point3Ds, std::vector<std::vector<size_t>> bonds)
: atoms(atom_count)
{
    size_t atom_num = 0;

    for (auto &atom : point3Ds)
    {
        atoms[atom_num++] = std::make_shared<Atom>(atom);
        // std::cout << atom_num << '\n';
    }

    for (auto &&bond : bonds)
    {
        size_t from, to, type;
        from = bond[0] - 1;
        to = bond[1] - 1;
        type = bond[2];
        add_bidirectional_bond(from, to, type);
    }
}

void Graph::add_bidirectional_bond(size_t a1, size_t a2, size_t type)
{
    bonds.push_back(std::make_shared<Bond>(atoms[a1], atoms[a2], type));
    bonds.push_back(std::make_shared<Bond>(atoms[a2], atoms[a1], type));
}

std::vector<std::shared_ptr<Bond>> Graph::find_bonds_of_atom(std::shared_ptr<Atom> atom)
{
    std::vector<std::shared_ptr<Bond>> ret;

    for (auto &&bond : bonds)
        if (bond->from.lock() == atom)
            ret.push_back(bond);

    return ret;
}

void Graph::print_graph()
{
    size_t atom_num = 1;
    for (auto &&atom : atoms)
    {
        std::cout << atom_num++ << ')' << *atom;
        for (auto &bond : find_bonds_of_atom(atom))
        {
            std::cout << *(bond.get()->to.lock());
        }
        std::cout << '\n';
    }
}

void Graph::delete_hanging_atoms()
{
    // size_t atom_num = 0;
    std::set<std::shared_ptr<Bond>> bonds_to_delete;
    std::set<std::shared_ptr<Atom>> atoms_to_delete;

    for (auto &&atom : atoms)
        // atom_num++;
        if (auto bonds = find_bonds_of_atom(atom); bonds.size() == 1)
        {
            // std::cout << atom_num << '\n';
            for (auto &&bond : bonds)
                bonds_to_delete.insert(bond);
            atoms_to_delete.insert(atom);
        }
    
    /* auto eatoms = */ std::erase_if(atoms, [&atoms_to_delete](std::shared_ptr<Atom> atom) 
        { return  atoms_to_delete.contains(atom); }
    );
    atoms_to_delete.clear();

    /* auto ebonds =  */std::erase_if(bonds, [&bonds_to_delete](std::shared_ptr<Bond> bond) 
        { return  bonds_to_delete.contains(bond) || bond->to.expired(); }
    );
    
    // std::cout << ebonds << ", " << eatoms << '\n';
    // std::cout << atoms.size() << ", " << bonds.size() << '\n';

}

void Graph::find_cycles()
{
}