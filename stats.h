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
            output_json.push_back({{"student input_file_id:",
                                   gc.nodes().college_student(edge.college_student_id).data().input_uuid},
                                   {"pupil uuid:",gc.nodes().pupil(edge.pupil_id).data().input_uuid}});
        }
        std::ofstream out("../examples/first_result.json");
        out<<output_json;
    }

    unsigned get_same_subject_number(const CollegeStudent & student, const Pupil & pupil,
            std::map<Subject, unsigned> & num_subject_request_fulfilled ) {
        unsigned retval = 0;
        for (auto const & student_subject : student.data().offered_subjects) {
            for (auto const & pupil_subject : pupil.data().requested_subjects) {
                if (student_subject.subject == pupil_subject.subject) {
                    if (num_subject_request_fulfilled.find(pupil_subject.subject) != std::end(num_subject_request_fulfilled)) {
                        num_subject_request_fulfilled[pupil_subject.subject]++;
                    } else {
                        num_subject_request_fulfilled[pupil_subject.subject] = 1;
                    }
                    retval++;
                    break;
                }
            }
        }
        return retval;
    }

    void collect_subject_requests_offers(const GraphCreator & gc, std::map<Subject, unsigned> & num_sub_req,
            std::map<Subject, unsigned> & num_sub_off) {
        for (auto const & student : gc.nodes().college_students()) {
            for (auto const & subject : student.data().offered_subjects) {
                if (num_sub_off.find(subject.subject) != std::end(num_sub_off)) {
                    num_sub_off[subject.subject]++;
                }else {
                    num_sub_off[subject.subject] = 1;
                }
            }
        }
        for (auto const & pupil : gc.nodes().pupils()) {
            for (auto const & subject : pupil.data().requested_subjects) {
                if (num_sub_req.find(subject.subject) != std::end(num_sub_req)) {
                    num_sub_req[subject.subject]++;
                }else {
                    num_sub_req[subject.subject] = 1;
                }
            }
        }
    }

    void dump_stats(std::vector<Edge> const & matching_edges, const GraphCreator & gc, double matching_cost) {
        json output_json;
        std::map<Subject, std::string> subject_to_string = {{Deutsch, "Deutsch"}, {Mathematik, "Mathematik"},
                                                                            {Englisch, "Englisch"}, {Physik, "Physik"}, {Chemie, "Chemie"},
                                                                             {Biologie, "Biologie"},{Informatik, "Informatik"},
                                                                              {Franzoesisch,"Französisch"},{Spanisch, "Spanisch"},
                                                                               {Latein, "Latein"}, {Erdkunde, "Erdkunde"}, {Sozialkunde, "Sozialkunde"},
                                                            {Geschichte, "Geschichte"}, {Wirtschaft, "Wirtschaft"}, {Politik, "Politik"}, {Philosophie, "Philosophie"},
                                                             {Musik, "Musik"}, {Kunst, "Kunst"}, {Paedagogik, "Pädagogik"}, {Religion,"Religion"},
                                                                 {Altgriechisch, "Altgriechisch"},{Italienisch, "Italienisch"},{Russisch, "Russisch"},
                                                                 {Niederlaendisch, "Niederländisch"},  {Chinesisch, "Chinesisch"}};
        std::map<Subject, unsigned> num_subject_offered;
        std::map<Subject, unsigned> num_subject_requested;
        std::map<Subject, unsigned> num_subject_request_fulfilled;
        collect_subject_requests_offers(gc, num_subject_requested, num_subject_offered);
        output_json.push_back({"Number of students", gc.nodes().college_students().size()});
        output_json.push_back({"Number of pupils", gc.nodes().pupils().size()});
        output_json.push_back({"Total number of edges in the graph", gc.edges().size()});
        output_json.push_back({"Total number of found matches", matching_edges.size()});
        output_json.push_back({"Matching Cost:", matching_cost});
        ///Costs by component
        CostValue bundesland_cost = 0.;
        CostValue fach_cost = 0.;
        CostValue waiting_cost = 0.;
        double avg_pupil_waiting_days = 0.;
        unsigned num_cov_subjects = 0;
        unsigned num_uncov_sub = 0;
        double most_waiting_days = 0;
        unsigned num_offered_subjects = 0;
        unsigned num_bund_match = 0;
        std::vector<bool> pupil_matched(gc.nodes().pupils().size(), false);
        for (auto const & edge : matching_edges) {
            auto const &student = gc.nodes().college_student(edge.college_student_id);
            auto const &pupil = gc.nodes().pupil(edge.pupil_id);
            pupil_matched[pupil.id()] = true;
            bundesland_cost += gc.cost_computer().get_specific_edge_cost(student, pupil, CostType::BundeslandBonus);
            fach_cost += gc.cost_computer().get_specific_edge_cost(student, pupil, CostType::FachUebereinstimmung);
            waiting_cost += gc.cost_computer().get_specific_edge_cost(student, pupil, CostType::WaitingTimeBonus);
            avg_pupil_waiting_days += pupil.data().waiting_days;
            auto const same_sub_num = get_same_subject_number(student, pupil, num_subject_request_fulfilled);;
            num_cov_subjects += same_sub_num;
            num_uncov_sub += pupil.data().requested_subjects.size() - same_sub_num;
            if (student.data().bundesland != Bundesland::Invalid and student.data().bundesland == pupil.data().bundesland) {
                num_bund_match++;
            }
        }
        avg_pupil_waiting_days /= matching_edges.size();
        for (unsigned pupil_id = 0; pupil_id < gc.nodes().pupils().size(); ++pupil_id) {
            if (not pupil_matched[pupil_id]) {
                most_waiting_days = std::max(most_waiting_days, gc.nodes().pupil(pupil_id).data().waiting_days);
                num_uncov_sub += gc.nodes().pupil(pupil_id).data().requested_subjects.size();
            }
        }
        for (auto const &[key, val] : num_subject_offered) {
            num_offered_subjects+= val;
        }
        output_json.push_back({"Average waiting days of a matched pupil", avg_pupil_waiting_days});
        output_json.push_back({"Most waiting days of an unmatched pupil", most_waiting_days});
        output_json.push_back({"Total number of covered subjects", num_cov_subjects});
        output_json.push_back({"Total number of uncovered subjects", num_uncov_sub});
        output_json.push_back({"Total number of offered subjects", num_offered_subjects});
        output_json.push_back({"Total number of matching edges with matching bundesland", num_bund_match});
        for (auto const & [key, val] : num_subject_offered) {
            output_json.push_back({subject_to_string[key], {{"offered", val}, {"requested", num_subject_requested[key]},
                                                            {"requests fulfilled", num_subject_request_fulfilled[key]}}});
        }
        std::ofstream out("../examples/first_stats.json");
        out<<output_json;
    }
}

#endif //CORONA_SCHOOL_MATCHING_STATS_H
