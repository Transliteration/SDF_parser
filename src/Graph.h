#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <set>
#include <map>

#include "Point3D.h"
#include "SDFFileParser.h"
#include "Atom.h"
#include "Bond.h"

struct Graph
{
    std::vector<std::shared_ptr<Atom>> atoms;
    std::vector<std::shared_ptr<Bond>> bonds;

    Graph(size_t atom_count, std::vector<Atom> atoms, std::vector<std::vector<size_t>> bonds);

    void add_bidirectional_bond(size_t a1, size_t a2, size_t type);

    std::vector<std::shared_ptr<Bond>> find_bonds_of_atom(std::shared_ptr<Atom> atom, std::shared_ptr<Bond> except = nullptr);

    void print_graph();

    void delete_hanging_atoms();

    void find_cycles();
    void print_graph_info();

    void bfs(std::shared_ptr<Atom> atom,
             std::map<std::shared_ptr<Atom>, std::shared_ptr<Bond>> &visited_from,
             std::set<std::shared_ptr<Bond>> &found_bonds,
             std::shared_ptr<Bond> prev_bond);
};