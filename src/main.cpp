#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <vector>
#include <memory>
#include <set>
#include <iomanip>

struct Point3D
{
    double x,y,z;
};

class Atom;

struct Bond
{
    std::weak_ptr<Atom> from, to;
    int type;

    Bond(std::weak_ptr<Atom> from, std::weak_ptr<Atom> to, int type)
    : from(from)
    , to(to)
    , type(type)
    {}
};


struct Atom
{
    union 
    {
        Point3D coords;
        struct {double x,y,z;};
    };

    Atom(Point3D &coords)
    : coords(coords)
    {}

    Atom() {}
};

std::ostream& operator<<(std::ostream& os, Atom &a)
{
    os << std::setw(8) << a.x << std::setw(8) << a.y << std::setw(8) << a.z << '\n';
    return os;
}

std::ostream& operator<<(std::ostream& os, Bond &bond)
{
    os << '\t' << *(bond.from.lock().get()) << "\t" << *(bond.to.lock().get());
    return os;
}

struct Graph
{
    std::vector<std::shared_ptr<Atom>> atoms;
    std::vector<std::shared_ptr<Bond>> bonds;

    Graph(size_t atom_count, std::vector<Point3D> point3Ds, std::vector<std::vector<size_t>> bonds)
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

    void add_bidirectional_bond(size_t a1, size_t a2, size_t type)
    {
        bonds.push_back(std::make_shared<Bond>(atoms[a1], atoms[a2], type));
        bonds.push_back(std::make_shared<Bond>(atoms[a2], atoms[a1], type));
    }

    std::vector<std::shared_ptr<Bond>> find_bonds_of_atom(std::shared_ptr<Atom> atom)
    {
        std::vector<std::shared_ptr<Bond>> ret;

        for (auto &&bond : bonds)
            if (bond->from.lock() == atom)
                ret.push_back(bond);

        return ret;
    }

    void print_graph()
    {
        size_t atom_num = 1;
        for (auto &&atom : atoms)
        {
            std::cout << atom_num++ << ')' << *atom << '\n';
            for (auto &bond : find_bonds_of_atom(atom))
            {
                std::cout << *(bond.get()) << '\n';
            }
            std::cout << '\n';
        }
    }

    void print_hanging_atoms()
    {
        size_t atom_num = 0;
        for (auto &&atom : atoms)
        {
            atom_num++;
            if (find_bonds_of_atom(atom).size() == 1)
                std::cout << atom_num << '\n';
            
        }
    }

    void print_bonds()
    {
        size_t atom_num = 1;
        for (auto &&atom : atoms)
        {
            std::cout << atom.use_count() << ' ' << atom_num++ << '\n';
        }
    }

    void find_cycles()
    {
    }

};


struct SDFFileParser
{
    std::ifstream file;
    std::array<int, 8> counts_line;
    std::vector<Point3D> point3Ds;
    std::vector<std::vector<size_t>> bonds;


    SDFFileParser(std::string &&filename)
    : file(filename)
    {
        if (!file.is_open())
        {
            std::cerr << "Unable to open file\n"; 
            return;
        }
    }

    ~SDFFileParser()
    {
        file.close();
    }

    bool parse_sdf_file()
    {
        read_header();
        read_counts_line();
        read_atom_block();
        read_bond_block();
        
        return true;
    }

    Graph build_graph()
    {
        Graph graph(counts_line[0], point3Ds, bonds);

        return graph;
    }

private:
    bool read_header()
    {
        std::string line;
        // skip first lines
        for (size_t i = 0; i < 3; i++)
            getline(file,line);
        
        return true;
    }

    bool read_counts_line()
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

    bool read_atom_block()
    {
        std::string atom_line;

        // std::cout << "Atom block:\n";
        for (size_t i = 0; i < counts_line[0]; i++)
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

    bool read_bond_block()
    {
        std::string bond_line;

        // std::cout << "Bond block:\n";
        for (size_t i = 0; i < counts_line[1]; i++)
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

};




int main()
{
    SDFFileParser parser("data/Conformer3D_CID_2244.sdf");
    parser.parse_sdf_file();  

    // std::cout << sizeof(Point3D);
    Graph graph = parser.build_graph();

    // graph.print_bonds();
    graph.print_hanging_atoms();
    // graph.print_bonds();
    // graph.print_graph();
    // graph.find_cycles();
    // graph.print_graph();
    return 0;
}