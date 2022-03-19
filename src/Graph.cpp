#include "Graph.h"
#include <deque>


Graph::Graph(size_t atom_count, std::vector<Atom> atoms, std::vector<std::vector<size_t>> bonds)
: atoms(atom_count)
{
    size_t atom_num = 0;

    for (auto &atom : atoms)
    {
        this->atoms[atom_num++] = std::make_shared<Atom>(atom);
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
    // bonds.push_back(std::make_shared<Bond>(atoms[a2], atoms[a1], type));
}

std::vector<std::shared_ptr<Bond>> Graph::find_bonds_of_atom(std::shared_ptr<Atom> atom, std::shared_ptr<Bond> except)
{
    std::vector<std::shared_ptr<Bond>> ret;

    for (auto &&bond : bonds)
        if ((bond->from.lock() == atom || bond->to.lock() == atom) && bond != except)
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
            std::cout << *bond;
        }
        std::cout << '\n';
    }
}

void Graph::print_graph_info()
{
    std::cout << "Graph\n" 
    << "Atoms count:" << atoms.size() << '\n' 
    <<"Bonds count:"  << bonds.size() << '\n';
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

void Graph::bfs(std::shared_ptr<Atom> curr_atom, 
                std::map<std::shared_ptr<Atom>, std::shared_ptr<Bond>> &visited_atoms, 
                std::set<std::shared_ptr<Bond>> &marked_atoms)
{

    // deque for atoms bound to curr_atom
    std::deque<std::shared_ptr<Atom>> atom_queue;

    // add curr_atom and bond, which lead to curr_atom, to map
    visited_atoms.insert({curr_atom, nullptr});

    curr_atom = atoms[0];
    std::shared_ptr<Bond> prev_bond = nullptr;
    while (atom_queue.size() > 0 || prev_bond == nullptr)
    {
        // std::cout << "curr_atom=" << *curr_atom;
        // add neighboring atoms to deque
        for (auto &&bond : find_bonds_of_atom(curr_atom, prev_bond))
        {
            if (auto next_atom = bond->to.lock(); !visited_atoms.contains(next_atom))
            {
                atom_queue.push_back(next_atom);
                visited_atoms.insert({next_atom, bond});
                // std::cout << "ADDED: " << *next_atom << *bond;
            }
            else if (auto next_atom = bond->from.lock(); !visited_atoms.contains(next_atom))
            {
                atom_queue.push_back(next_atom);
                visited_atoms.insert({next_atom, bond});
                // std::cout << "ADDED: " << *next_atom << *bond;
            }
            else
            {
                // std::cout << "\t\tWTF!\n";
                // visited_atoms.insert({curr_atom, bond});
                marked_atoms.insert(bond);
                // std::cout << "WTF INFO: " << *curr_atom << *bond;
            }

        }
        // std::cout << visited_atoms.size() << '\n';
        // std::cout << "Atom queue size:" << atom_queue.size() << '\n';
        if (atom_queue.size() > 0)
        {
            curr_atom = atom_queue.front();
            prev_bond = visited_atoms.find(curr_atom)->second;
            atom_queue.pop_front();
        }
    }
    
    
    
}

void Graph::find_cycles()
{
    std::map<std::shared_ptr<Atom>, std::shared_ptr<Bond>> visited_atoms; // assosiate atom with bond from which it was reached
    std::set<std::shared_ptr<Bond>> found_bonds; // store bonds which lead to already iterated atom by bfs

    bfs(atoms[0], visited_atoms, found_bonds);

    std::set<std::shared_ptr<Bond>> marked_bonds; // store bonds which belong to cycle (for deletion)
    // std::cout << "Visited Atoms:" << visited_atoms.size() << '\n';
    for (auto &&[atom, bond] : visited_atoms)
    {
        if (visited_atoms.count(atom) == 1) continue;
        // std::cout << *atom;

        // check for input atom (which has no prev_bond)
        if (bond != nullptr)
            std::cout << *bond;
    }

    std::cout << "Marked count:" << found_bonds.size() << '\n';

    // for each bond used times
    for (auto &&bond : found_bonds)
    {
        // mark for deletion
        marked_bonds.insert(bond);
        // create atoms to iterate backwards through cycle
        std::shared_ptr<Atom> atom  = bond->from.lock(),
                              atom2 = bond->to.lock();

        // store atom found in cycle
        std::set<std::shared_ptr<Atom>> path;
        path.insert(atom2);
        path.insert(atom);

        //[1] found which bond lead to atom (a),
        //[2] mark this bond for deletion,
        //[3] return atom from other side of bond (iterate backwars) 
        auto find_prev_atom_and_mark_bond = [&visited_atoms, &marked_bonds](auto a)->auto { 
            std::shared_ptr<Atom> ret = nullptr; // if 
            if (a == nullptr || !visited_atoms.contains(a)) 
            {
                std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n\n\n\n";
                return ret;
            }

            std::shared_ptr<Bond> bond = visited_atoms.find(a)->second; // [1]
            if (bond != nullptr)
            {
                marked_bonds.insert(bond); // [2]
                if (bond->from.lock() == a)
                    return bond->to.lock(); // [3]
                else if (bond->to.lock() == a)
                    return bond->from.lock(); // [3]
            }
            std::cout << "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB\n\n\n\n";
            return ret; 
        };

        int cycle_max_len = atoms.size();
        for (int i = 0; i < cycle_max_len; i++)
        {
            if (atom2 == atom)
            {
                // std::cout << "Found cycle! Length: " << i << '\n';
                // std::cout << *atom << *atom2;
                break;
            }
            // iterate atoms backwards one at a time
            if (i%2 == 0)
            {
                atom2 = find_prev_atom_and_mark_bond(atom2);
                path.insert(atom2);
            }
            else
            {
                atom = find_prev_atom_and_mark_bond(atom);
                path.insert(atom);
            }
        }
        // print all atoms in found cycle
        // for (auto &&atom : path)
        // {
        //     if (atom != nullptr)
        //         std::cout << *atom;
        //     else
        //         std::cout << "nullptr\n";
        // }
        // std::cout << "END of cycle\n";
        
    }

    // erase marked bonds
    std::erase_if(bonds, [&marked_bonds](std::shared_ptr<Bond> bond) 
        { return  marked_bonds.contains(bond); }
    );
    // clear temporary containers
    marked_bonds.clear();
    visited_atoms.clear();
    found_bonds.clear();

    // delete atoms without bonds
    std::erase_if(atoms, [this](std::shared_ptr<Atom> atom) 
        { return  find_bonds_of_atom(atom).size() == 0; }
    );
}