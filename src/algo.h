#ifndef CORONA_SCHOOL_MATCHING_ALGO_H
#define CORONA_SCHOOL_MATCHING_ALGO_H

#include "graph_creator.h"
#include "stats.h"
#include "matching.h"
#include <iostream>

namespace CS {
    void perform_algo_with_file_names(char const * pupil_file, char const * student_file,
                                      char const * balancing_coef, char const * matching_file, char const * stats_file) {
        CS::GraphCreator gc;
        ///We assume that the first file encodes the pupils, the second one the students and the third one the balancing coefficients.
        std::ifstream pupil_input_file_stream(pupil_file);
        std::ifstream student_input_file_stream(student_file);
        std::ifstream balancing_coefficients(balancing_coef);
        gc.init_from_json(pupil_input_file_stream, student_input_file_stream, balancing_coefficients, std::nullopt, std::nullopt);
        std::vector<CS::Edge> matching_edges;
        auto const matching_cost = CS::compute_max_cost_matching(gc, matching_edges, CS::MatchingAlgorithm::SuccessiveShortestPath);
        CS::test_matching_valid(matching_edges, gc.nodes());
        ///The fourth file should encode the json output file for the matches
        CS::dump_matching_edges_into_json(matching_edges, gc, std::string(matching_file));
        ///The fifth file should encode the json output file for the stats
        CS::dump_stats(matching_edges, gc, matching_cost, std::string(stats_file));
    }
}

#endif //CORONA_SCHOOL_MATCHING_ALGO_H
