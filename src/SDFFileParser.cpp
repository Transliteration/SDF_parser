#include "SDFFileParser.h"

#include "Graph.h"

SDFFileParser::SDFFileParser(std::string &&filename)
: file(filename)
{
    if (!file.is_open())
    {
        std::cerr << "Unable to open file\n"; 
        return;
    }
}

SDFFileParser::~SDFFileParser()
{
    file.close();
}

bool SDFFileParser::parse_sdf_file()
{
    read_header();
    read_counts_line();
    read_atom_block();
    read_bond_block();
    
    return true;
}

Graph SDFFileParser::build_graph()
{
    Graph graph(counts_line[0], point3Ds, bonds);

    return graph;
}

bool SDFFileParser::read_header()
{
    std::string line;
    // skip first lines
    for (size_t i = 0; i < 3; i++)
        getline(file,line);
    
    return true;
}

bool SDFFileParser::read_counts_line()
{
    std::string line;

    getline(file,line);
    std::stringstream ss(line); 

    // std::cout << "Counts line:\n";
    for (size_t i = 0; i < 8; i++)
    {
        ss >> counts_line[i];
        // std::cout << counts_line[i] << " ";
    }
    // std::cout << "\n\n";

    return true;
}

bool SDFFileParser::read_atom_block()
{
    std::string atom_line;

    // std::cout << "Atom block:\n";
    for (int i = 0; i < counts_line[0]; i++)
    {
        getline(file,atom_line);
        std::stringstream ss(atom_line); 

        double x, y, z;
        char atom_name;
        ss >> x >> y >> z >> atom_name;
        // if (atom_name == 'H') continue;
        point3Ds.push_back({x, y, z});
        // std::cout << x << ", " << y << ", " << z << "\n";
    }
    
    
    // std::cout << point3Ds.size() << "\n";
    return true;
}

bool SDFFileParser::read_bond_block()
{
    std::string bond_line;

    // std::cout << "Bond block:\n";
    for (int i = 0; i < counts_line[1]; i++)
    {
        getline(file,bond_line);
        std::stringstream ss(bond_line); 

        size_t p1, p2, type;
        ss >> p1 >> p2 >> type;
        bonds.push_back({p1, p2, type});
        // std::cout << p1 << ", " << p2 << ", " << type << "\n";
    }
    
    
    // std::cout << bonds.size() << "\n";

    return true;
}
