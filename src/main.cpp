#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <vector>
#include <memory>
#include <set>

struct Point3D
{
    double x,y,z;
};

class Atom;

struct Bond
{
    std::weak_ptr<Atom> atom;
    // Atom* atom;
    int type;

    Bond(std::weak_ptr<Atom> a, int type)
    : atom(a)
    , type(type)
    {}
};


struct Atom
{
    std::vector<Bond> bondTo;
    bool visited = false; 
    bool marked_to_delete = false;
    std::weak_ptr<Atom> visited_from;
    int vis_count = 0;

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
    os << a.x << "\t" << a.y << "\t" << a.z << "\t" << a.visited;
    if (!a.visited_from.expired())
        os << '\t' << a.visited_from.lock()->x;
    os << '\t' << a.vis_count;
    return os;
}

std::ostream& operator<<(std::ostream& os, Bond &bond)
{
    os << bond.atom.lock()->x << "\t" << bond.atom.lock()->y << "\t" << bond.atom.lock()->z;
    return os;
}

struct Graph
{
    std::vector<std::shared_ptr<Atom>> atoms;

    Graph(size_t atom_num)
    : atoms(atom_num)
    {
        // std::cout << atoms.size() << "\n";
        // std::cout << atoms.capacity() << "\n";
    }

    void add_bidirectional_bond(size_t a1, size_t a2, size_t type)
    {
        atoms[a1]->bondTo.push_back(Bond(atoms[a2],type));
        atoms[a2]->bondTo.push_back(Bond(atoms[a1],type));
    }

    void print_graph()
    {
        size_t atom_num = 1;
        for (auto &&atom : atoms)
        {
            std::cout << atom_num++ << ')' << *atom << '\n';
            for (auto &bond : atom->bondTo)
            {
                std::cout << "\t  " << bond << '\n';
            }
            std::cout << '\n';
        }
    }

    void print_hanging_atoms()
    {
        size_t atom_num = 1;

        std::vector<size_t> atoms_to_delete;

        for (int i = atoms.size() - 1; i >= 0; i--)
        {
            if (atoms[i]->bondTo.size() == 1) 
            {
                atoms_to_delete.push_back(i);   
                auto &&temp_bonds = atoms[i]->bondTo[0].atom.lock()->bondTo;
                for (size_t k = 0; k < temp_bonds.size(); k++)
                {
                    if (temp_bonds[k].atom.lock()->x == atoms[i]->x)
                    {
                        std::cout << "equal\n";
                        temp_bonds.erase(temp_bonds.begin() + k);
                        break;
                    } 
                }
            }
        }

        for (auto &&i : atoms_to_delete)
        {
            // for (auto &j : atoms[i]->bondTo)
            // {
            //     j.atom.lock()->bondTo.    
            // }
            
            atoms.erase(atoms.begin()+i);
        }
        
        for (auto &&atom : atoms)
        {
            if (atom->bondTo.size() == 1)
                std::cout << *atom << " " << atom_num << '\n';
            atom_num++;
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
        std::weak_ptr<Atom> cur_atom = atoms[0];
        cur_atom.lock()->visited = true;
        cur_atom.lock()->vis_count++;
        // cur_atom.lock()->visited_from = cur_atom;
        std::vector<std::weak_ptr<Atom>> next_atoms;
        for (auto &&bond : cur_atom.lock()->bondTo)
        {
            if (bond.atom.lock()->visited == false)
            {
                bond.atom.lock()->visited_from = cur_atom;
                // bond.atom->visited = true;
                bond.atom.lock()->vis_count++;
                next_atoms.push_back(bond.atom);
            }
        }

        std::set<std::shared_ptr<Atom>> cycle_ends;

        size_t op_count = 0;

        while (next_atoms.size() != 0)
        {
            
            cur_atom = next_atoms.back();
            next_atoms.pop_back();
            if (cur_atom.lock()->visited) continue;
            cur_atom.lock()->visited = true;
            for (auto &&bond : cur_atom.lock()->bondTo)
            {
                op_count++;
                if (bond.atom.lock()->visited == false)
                {
                    bond.atom.lock()->visited_from = cur_atom;
                    bond.atom.lock()->vis_count++;
                    // bond.atom->visited = true;
                    if (!bond.atom.lock()->visited)
                        next_atoms.push_back(bond.atom);
                }
                else
                {
                    cur_atom.lock()->vis_count++;
                    if (cur_atom.lock()->vis_count > 2)
                        cycle_ends.insert(cur_atom.lock());
                    // cur_atom.lock()->visited_from->marked_to_delete = true;
                    cur_atom.lock()->visited_from.lock()->marked_to_delete = true;
                }
            }
        }  

        std::cout << "OP=" << op_count << '\n' << "SIZE=" << cycle_ends.size() << '\n';

        // std::set<std::weak_ptr<Atom>> cycle;

        for (auto atom : cycle_ends)
        {
            while (atom->visited_from.lock()->marked_to_delete)
            {
                atom->marked_to_delete = true;
                atom = atom->visited_from.lock();
                std::cout << "1\n";
            }
            std::cout << *(atom.get()) << "-------<Atom\n";

        }

            
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
        Graph graph(counts_line[0]);

        size_t atom_num = 0;

        for (auto &atom : point3Ds)
        {
            graph.atoms[atom_num++] = std::make_shared<Atom>(atom);
            // std::cout << atom_num << '\n';
        }

        for (auto &&bond : bonds)
        {
            size_t from, to, type;
            from = bond[0] - 1;
            to = bond[1] - 1;
            type = bond[2];
            graph.add_bidirectional_bond(from, to, type);
        }
        
        

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
    graph.print_graph();
    graph.find_cycles();
    graph.print_graph();
    return 0;
}