#include <iostream>
#include "graph_creator.h"
#include "matching.h"
#include "nlohmann/json.hpp"
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
    write_example_into_json();
    CS::GraphCreator gc;
    std::ifstream fstream("../examples/first_input.json");
    gc.init_from_json(fstream);
    std::vector<CS::Edge> matching_edges;
    CS::compute_max_cost_matching(gc, matching_edges);
    for (auto const & edge : matching_edges) {
        std::cout<<edge.pupil_id <<" "<<edge.college_student_id<<std::endl;
    }
    //ADAPT PRICES MAYBE AND RESTART
    return 0;
}