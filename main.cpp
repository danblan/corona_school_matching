#include <iostream>
#include "graph_creator.h"
#include "matching.h"
#include "nlohmann/json.hpp"
#include "stats.h"
using json = nlohmann::json;


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
int main(int argc, char * argv[]) {
    //READ DATA AND CONSTRUCT GRAPH
    if (argc != 4) {
        std::cout<<"Wrong number of input files, we need 3 of them!"<<std::endl;
        return EXIT_FAILURE;
    }
    CS::GraphCreator gc;
    ///We assume that the first file encodes the pupils, the second one the students and the third one the balancing coefficients.
    std::ifstream pupil_input_file_stream(argv[1]);
    std::ifstream student_input_file_stream(argv[2]);
    std::ifstream balancing_coefficients(argv[3]);
    gc.init_from_json(pupil_input_file_stream, student_input_file_stream, balancing_coefficients, std::nullopt, std::nullopt);
    std::vector<CS::Edge> matching_edges;
    auto const matching_cost = CS::compute_max_cost_matching(gc, matching_edges, CS::MatchingAlgorithm::SuccessiveShortestPath);
    CS::test_matching_valid(matching_edges, gc.nodes());
    CS::dump_matching_edges_into_json(matching_edges, gc);
    //ADAPT PRICES MAYBE AND RESTART
    return 0;
}