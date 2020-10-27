#include <iostream>
#include "graph_creator.h"
#include "nlohmann/json.hpp"
#include <algorithm>
#include <iomanip>

using json = nlohmann::json;

namespace CS {
    //Helper method to convert a string encoding a state into the suiting enum.
    Bundesland parse_bundesland(std::string bl) {
        ///Convert the string to lower case:
        std::for_each(bl.begin(), bl.end(), [](char &c) {
            c = std::tolower(c);
        });
        if (bl == "bb") return Brandenburg;
        if (bl == "be") return Berlin;
        if (bl == "bw") return BadenWuerttemberg;
        if (bl == "by") return Bayern;
        if (bl == "hb") return Bremen;
        if (bl == "he") return Hessen;
        if (bl == "hh") return Hamburg;
        if (bl == "mv") return MecklemburgVorpommern;
        if (bl == "ni") return Niedersachsen;
        if (bl == "nw") return NordrheinWestfalen;
        if (bl == "rp") return RheinlandPfalz;
        if (bl == "sh") return SchleswigHolstein;
        if (bl == "sl") return Saarland;
        if (bl == "sn") return Sachsen;
        if (bl == "st") return SachsenAnhalt;
        if (bl == "th") return Thueringen;
        return Invalid;
    }

    //Helper method converting a string encoding a subject into the suiting enum.
    Subject parse_subject(std::string s) {
        ///Convert the string to lower case:
        std::for_each(s.begin(), s.end(), [](char &c) {
            c = std::tolower(c);
        });
        if (s == "mathematik") return Mathematik;
        if (s == "deutsch") return Deutsch;
        if (s == "englisch") return Englisch;
        if (s == "geschichte") return Geschichte;
        if (s == "chemie") return Chemie;
        if (s == "physik") return Physik;
        if (s == "biologie") return Biologie;
        if (s == "sozialkunde") return Sozialkunde;
        if (s == "spanisch") return Spanisch;
        if (s == "französisch") return Franzoesisch;
        if (s == "informatik") return Informatik;
        if (s == "erdkunde") return Erdkunde;
        if (s == "latein") return Latein;
        if (s == "wirtschaft") return Wirtschaft;
        if (s == "politik") return Politik;
        if (s == "philosophie") return Philosophie;
        if (s == "musik") return Musik;
        if (s == "kunst") return Kunst;
        if (s == "pädagogik") return Paedagogik;
        if (s == "religion") return Religion;
        if (s == "altgriechisch") return Altgriechisch;
        if (s == "italienisch") return Italienisch;
        if (s == "russisch") return Russisch;
        if (s == "niederländisch") return Niederlaendisch;
        if (s == "chinesisch") return Chinesisch;
        else std::cout << "subject not identified, name was" << s << std::endl;
    }

    CostType parse_cost_type(std::string s) {
        std::for_each(s.begin(), s.end(), [](char &c) {
            c = std::tolower(c);
        });
        if (s == "bundeslandbonus") return BundeslandBonus;
        if (s == "fachuebereinstimmung") return FachUebereinstimmung;
        if (s == "wartezeitbonus") return WaitingTimeBonus;
    }

    double get_day_difference_from_today(const std::string &date) {
        static constexpr unsigned SECONDS_IN_A_DAY = 86400;
        //Get time now
        std::time_t now = std::time(nullptr);
        //Convert string into time_t
        std::tm t = {};
        std::istringstream ss(date);
        ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");
        //Compute the time difference
        return std::difftime(now, std::mktime(&t)) / SECONDS_IN_A_DAY;
    }

    template <typename T>
    inline bool contains(const std::vector<T> &vec, T const &elem) {
        for (auto const & t : vec) {
            if (t == elem) {
                return true;
            }
        }
        return false;
    }


    void GraphCreator::init_from_json(std::ifstream &pupil_file, std::ifstream &student_file,
                                      std::ifstream &balancing_coefficients,
                                      std::optional<std::vector<std::string>> const & pupil_uuids,
                                      std::optional<std::vector<std::string>> const & student_uuids) {
        json pupil_data_file, student_data_file;
        //Initialize the json object:
        pupil_file >> pupil_data_file;
        student_file >> student_data_file;
        //Build pupils and students with the data in the json file.
        _nodes.pupils().reserve(pupil_data_file.size());
        unsigned pupil_count{0u};
        for (auto const &pupil_json_data : pupil_data_file) {
            if ((not pupil_uuids) or (not contains<std::string>(*pupil_uuids, pupil_json_data["uuid"]))) {
                //This pupil cannot be matched currently..
                continue;
            }
            _nodes.create_pupils(1u);
            auto &pupil_data = _nodes.pupil(pupil_count).data();
            ///bundesland not yet featured
            //pupil_data.bundesland = parse_bundesland(pupil_json_data["Bundesland"]);
            pupil_data.input_file_id = pupil_json_data["id"];
            pupil_data.grade = pupil_json_data["grade"];
            pupil_data.waiting_days = get_day_difference_from_today(pupil_json_data["createdAt"]);
            for (auto const &fach : pupil_json_data["subjects"]) {
                pupil_data.requested_subjects.emplace_back(parse_subject(fach["name"]));
            }
            pupil_count++;
        }

        _nodes.college_students().reserve(student_data_file.size());
        unsigned student_count{0u};
        for (auto const &student_json_data : student_data_file) {
            if ((not student_uuids) or (not contains<std::string>(*student_uuids, student_json_data["uuid"]))) {
                //This student cannot be matched currently
                continue;
            }
            _nodes.create_college_students(1u);
            auto &student_data = _nodes.college_student(student_count).data();
            ///bundesland not yet featured
            student_data.bundesland = parse_bundesland(student_json_data["state"]);
            student_data.waiting_days = get_day_difference_from_today(student_json_data["createdAt"]);
            student_data.input_file_id = student_json_data["id"];
            for (auto const &offered_sub : student_json_data["subjects"]) {
                Subject const subject = parse_subject(offered_sub["name"]);
                GradeRange range;
                //Currently only with min and max
                unsigned min, max;
                if (offered_sub.find("grade") == std::end(offered_sub)) {
                    //Assume that in this case we have all grades
                    min = MIN_POSSIBLE_GRADE;
                    max = MAX_POSSIBLE_GRADE;
                } else {
                    min = offered_sub["grade"]["min"];
                    max = offered_sub["grade"]["max"];
                }
                for (unsigned grade = min; grade <= max; ++grade) {
                    range.grades.push_back(grade);
                }
                student_data.offered_subjects.push_back({subject, 1., range});
            }
            student_count++;
        }
        //Create edges:
        create_edges();
        //Initialize the cost components that are used currently:
        edge_cost_computer.add_bundesland_bonus();
        edge_cost_computer.add_fachuebereinstimmung();
        edge_cost_computer.add_waiting_time_bonus();
        //balance the edge costs
        balance_edge_costs(balancing_coefficients);
        //Cache the edge costs:
        init_edge_costs();
    }

    void GraphCreator::init_edge_costs() {

        //Iterate over all edges and cache the costs which are computed by the edge cost computer.
        for (auto &edge : _edges) {
            edge.cost = edge_cost_computer.compute_edge_cost(
                    _nodes.college_student(edge.college_student_id), _nodes.pupil(edge.pupil_id));
        }
    }

    void GraphCreator::create_edges() {
        for (auto const &pupil : _nodes.pupils()) {
            for (auto const &student : _nodes.college_students()) {
                if (pupil.accepts(student) and student.accepts(pupil)) {
                    _edges.emplace_back(pupil.id(), student.id());
                }
            }
        }
    }

    void GraphCreator::balance_edge_costs(std::ifstream &balancing_coefficient_file) {
        json balancing_coeff_json;
        //Initialize the json object:
        balancing_coefficient_file >> balancing_coeff_json;
        std::map<CostType, double> cost_coefficients_by_type;
        std::map<CostType, double> total_edge_costs_by_type;
        //Collect the coefficients in the json file:
        for (auto it = balancing_coeff_json.begin(); it != balancing_coeff_json.end(); ++it) {
            CostType const cost_type = parse_cost_type(it.key());
            cost_coefficients_by_type.emplace(cost_type, it.value());
        }
        //Compute the fraction of the total cost that is currently consumed by each cost component:
        double total_cost{0.};
        for (auto const &[type, coeff] : cost_coefficients_by_type) {
            double specific_cost_total{0.};
            for (auto const &edge : edges()) {
                auto const &student = nodes().college_student(edge.college_student_id);
                auto const &pupil = nodes().pupil(edge.pupil_id);
                auto const current_cost = edge_cost_computer.get_specific_edge_cost(student, pupil, type);
                total_cost += current_cost;
                specific_cost_total += current_cost;
            }
            total_edge_costs_by_type[type] = specific_cost_total;
        }
        if (total_cost == 0.) return;
        //Now adapt the coefficients:
        for (auto const &[type, cost] : total_edge_costs_by_type) {
            if (cost == 0.) continue;
            auto const current_coefficient = cost / total_cost;
            if (current_coefficient == 0.) continue;
            auto const adaption_factor = cost_coefficients_by_type[type] / current_coefficient;
            edge_cost_computer.set_cost_coefficient(type, adaption_factor * edge_cost_computer.cost_coefficient(type));
        }
    }
}


