#ifndef CORONA_SCHOOL_MATCHING_STATS_H
#define CORONA_SCHOOL_MATCHING_STATS_H
#include "graph_creator.h"
using json = nlohmann::json;

namespace CS {
    /**
     * Function that verifies that the edges contained in @param matching_edges form a valid matching,
     * i.e. we check that each pupil and stundent is covered at most once and every match is valid.
     * @param nodes The node container with information about students and pupils.
     */
    void test_matching_valid(const std::vector<Edge> &matching_edges, const NodeContainer & nodes) {
        std::vector<bool> student_covered(nodes.college_students().size(), false);
        std::vector<bool> pupil_covered(nodes.pupils().size(), false);
        for (auto const &edge : matching_edges) {
            auto const & student = nodes.college_students()[edge.college_student_id];
            auto const & pupil = nodes.pupils()[edge.pupil_id];
            //Check that pupil and student are not covered yet.
            if (pupil_covered[edge.pupil_id] or student_covered[edge.college_student_id]) {
                //TODO: Throw exceptions or write into log file.
                std::cout<<"ERROR, MATCHING COVERS A STUDENT OR PUPIL TWICE!"<<std::endl;
                break;
            }
            pupil_covered[edge.pupil_id] = true;
            student_covered[edge.college_student_id] = true;
            //Check that pupil and student accept each other.
            if ((not pupil.accepts(student)) or (not student.accepts(pupil))) {
                //TODO: Throw exceptions or write into log file.
                std::cout<<"ERROR, MATCHING CONTAINS AN EDGE THAT SHOULD NOT EXIST!"<<std::endl;
                break;
            }
        }
    }

    struct Stats {
        double average_num_matched_subjects = 0.;
        unsigned number_of_matches = 0;
    };

    /**
     * Function that writes the subjects of each pupil and student that are matched into a json file.
     * @param edges The matching edges.
     * @param gc The graph creator containing the information about students and pupils.
     */
    void dump_matching_edges_into_json(const std::vector<Edge> & edges, const GraphCreator & gc) {
        json output_json;
        for (auto const & edge : edges) {
            //Collect the offered and requested subjects and write them into a json file.
            std::vector<Subject> student_subjects;
            std::vector<Subject> pupil_subjects;
            for (auto const & offered_sub : gc.nodes().college_students()[edge.college_student_id].data().offered_subjects) {
                student_subjects.push_back(offered_sub.subject);
            }
            for (auto const &requested_sub : gc.nodes().pupils()[edge.pupil_id].data().requested_subjects) {
                pupil_subjects.push_back(requested_sub.subject);
            }
            output_json.push_back({{"student subjects:",
                                   student_subjects}, {"pupil subjects:", pupil_subjects}});
        }
        std::ofstream out("../examples/first_result.json");
        out<<output_json;
    }
}

#endif //CORONA_SCHOOL_MATCHING_STATS_H
