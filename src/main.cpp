#include <iostream>
#include "Graph.h"

#include "Visualizer.h"


int main()
{
    SDFFileParser parser("data/Conformer3D_CID_2244.sdf");
    // SDFFileParser parser("data/Phenyl salicylate (Compound).sdf"); 
    // SDFFileParser parser("data/Porphyrin.sdf");
    // SDFFileParser parser("data/Nicotine.sdf");
    parser.parse_sdf_file();  


    Graph graph = parser.build_graph();

    // graph.print_graph_info();
    graph.delete_hanging_atoms();
    // graph.print_graph_info();
    graph.find_cycles();
    // graph.print_graph_info();
    // graph.print_graph();

    Visualizer vis(graph.atoms, graph.bonds);

    vis.rotate_all_by_random_angles();
    // vis.find_transformation_with_low_error();


    

    return 0;
}