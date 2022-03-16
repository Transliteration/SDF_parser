#include <iostream>

#include "Graph.h"



int main()
{
    SDFFileParser parser("data/Conformer3D_CID_2244.sdf");
    parser.parse_sdf_file();  


    Graph graph = parser.build_graph();

    // graph.print_graph();
    graph.delete_hanging_atoms();
    // graph.print_graph();

    return 0;
}