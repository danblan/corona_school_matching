#include <iostream>
#include "graph_creator.h"
#include "matching.h"
#include "nlohmann/json.hpp"
#include "stats.h"
using json = nlohmann::json;

void write_example_into_json() {
    json example = {
            {"students", {
                    {{"Bundesland", "NRW"}, {"Fächer",{{ {"Fach","Deutsch"},{"Klassenstufe", {5,6,7,8}}}}}},
                    {{"Bundesland", "RLP"}, {"Fach", "Mathe"}}}
            },
            {"pupils", {
                    {{"Bundesland", "Hamburg"}, {"Klassenstufe", 6}},
                    {{"Bundesland", "RLP"}, {"Klassenstufe", 5}}}
            }
    };
    std::ofstream out("../examples/first.json");
    out<<example;
}

int main() {
    //READ DATA AND CONSTRUCT GRAPH
    ///Local test:
    //CS::GraphCreator gc;
    //std::ifstream pupfstream("C:/Users/danie/Downloads/data/data/datasets/05_08_2020_13_39_16/tutees.json");
    //std::ifstream stufstream("C:/Users/danie/Downloads/data/data/datasets/05_08_2020_13_39_16/tutors.json");
    //gc.init_from_json(pupfstream, stufstream);
    //std::vector<CS::Edge> matching_edges;
    //std::cout<< CS::compute_max_cost_matching(gc, matching_edges)<<std::endl;
    //CS::test_matching_valid(matching_edges, gc.nodes());
    //CS::dump_matching_edges_into_json(matching_edges, gc);
    //ADAPT PRICES MAYBE AND RESTART
    return 0;
}