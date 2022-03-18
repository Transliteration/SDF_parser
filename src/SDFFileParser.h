#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <vector>

#include "Point3D.h"
#include "Bond.h"

class Graph;

struct SDFFileParser
{
    std::ifstream file;
    std::array<int, 8> counts_line;
    std::vector<Atom> atoms;
    std::vector<std::vector<size_t>> bonds;


    SDFFileParser(std::string &&filename);
    ~SDFFileParser();

    bool parse_sdf_file();
    Graph build_graph();

private:

    bool read_header();
    bool read_counts_line();
    bool read_atom_block();
    bool read_bond_block();

};
