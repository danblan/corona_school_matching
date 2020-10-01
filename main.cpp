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

std::vector<std::string> get_uuids_of_pupils(std::ifstream & pupils_file) {
    std::vector<std::string> retval;
    std::string line;
    std::string token;
    std::getline(pupils_file, line);
    std::istringstream ss(line);
    getline(ss, token, ',');
    while(getline(ss, token, ',')){ }

    while(std::getline(pupils_file, line)) {
        std::istringstream lol(line);
        std::string tokenbeard;
        getline(lol, tokenbeard, ',');
        retval.push_back(tokenbeard);
        while (getline(lol, token, ',')) {}
    }
    return retval;
}

std::vector<std::string> get_uuids_of_students(std::ifstream & pupils_file) {
    std::vector<std::string> retval;
    std::string line;
    std::string token;
    std::getline(pupils_file, line);
    std::istringstream ss(line);
    getline(ss, token, ',');
    while(getline(ss, token, ',')){ }

    while (std::getline(pupils_file, line)) {
        std::istringstream lol(line);
        std::string tokenbeard;
        getline(lol, tokenbeard, ',');
        retval.push_back(tokenbeard);
        while (getline(lol, tokenbeard, ',')) {}
    }
    return retval;
}
int main() {
    //READ DATA AND CONSTRUCT GRAPH
    ///Local test:
    CS::GraphCreator gc;
    std::ifstream pupfstream("C:/Users/danie/CLionProjects/corona_school_matching/examples/20_08_2020_Export_2/20_08_2020_Export_2/datasets/20_08_2020_22_09_44/tutees.json");
    std::ifstream stufstream("C:/Users/danie//CLionProjects/corona_school_matching/examples/20_08_2020_Export_2/20_08_2020_Export_2/datasets/20_08_2020_22_09_44/tutors.json");
    std::ifstream pupil_csv("C:/Users/danie/Downloads/data/data/matchings/04_08_2020/input/pupils.csv");
    std::ifstream student_csv("C:/Users/danie/Downloads/data/data/matchings/04_08_2020/input/students_upper_bound.csv");
    std::ifstream balancing_coeff("C:/Users/danie/CLionProjects/corona_school_matching/balancing_coefficients.json");
    auto pupil_uuids = get_uuids_of_pupils(pupil_csv);
    auto student_uuids = get_uuids_of_students(student_csv);
    gc.init_from_json(pupfstream, stufstream, balancing_coeff, pupil_uuids, student_uuids);
    std::vector<CS::Edge> matching_edges;
    std::cout<< CS::compute_max_cost_matching(gc, matching_edges, CS::MatchingAlgorithm::SuccessiveShortestPath)<<std::endl;
    CS::test_matching_valid(matching_edges, gc.nodes());
    CS::dump_matching_edges_into_json(matching_edges, gc);
    //ADAPT PRICES MAYBE AND RESTART
    return 0;
}