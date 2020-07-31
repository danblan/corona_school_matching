#include <iostream>
#include "graph_creator.h"
#include "nlohmann/json.hpp"
#include <algorithm>
using json = nlohmann::json;

namespace CS{
    //Helper method to convert a string encoding a state into the suiting enum.
    Bundesland  parse_bundesland(std::string bl) {
        ///Convert the string to lower case:
        std::for_each(bl.begin(), bl.end(), [](char & c){
            c = std::tolower(c);
        });
        if (bl == "hamburg") return Hamburg;
        if (bl == "berlin") return Berlin;
        if (bl == "brandenburg") return Brandenburg;
        if (bl == "bremen") return Bremen;
        if (bl == "rheinland-pfalz") return RheinlandPfalz;
        if (bl == "nordrhein-westfahlen") return NordrheinWestfalen;
        if (bl == "saarland") return Saarland;
        if (bl == "sachsen") return Sachsen;
        if (bl == "sachsen-anhalt") return SachsenAnhalt;
        if (bl == "bayern") return Bayern;
        if (bl == "baden-württemberg") return BadenWuerttemberg;
        if (bl == "hessen") return Hessen;
        if (bl == "niedersachsen") return Niedersachsen;
        if (bl == "thüringen") return Thueringen;
        if (bl == "schleswig-holstein") return SchleswigHolstein;
        if (bl == "mecklemburg-vorpommern") return MecklemburgVorpommern;
        return Bayern;
    }

    //Helper method converting a string encoding a subject into the suiting enum.
    Subject parse_subject(std::string s) {
        ///Convert the string to lower case:
        std::for_each(s.begin(), s.end(), [](char & c){
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

    }

    //TODO: Add some safety checks in this function
    void GraphCreator::init_from_json(std::ifstream & input) {
        json data_file;
        //Initialize the json object:
        input >> data_file;
        if (data_file.find("pupils") == std::end(data_file)) {
            throw std::invalid_argument(std::string("Input JSON file has no pupil field!"));
        } else if (data_file.find("students") == std::end(data_file)) {
            throw std::invalid_argument(std::string("Input JSON file has no student field"));
        }
        //Build pupils and students with the data in the json file.
        _nodes.create_pupils(data_file["pupils"].size());
        unsigned pupil_count{0u};
        for (auto const & pupil_json_data : data_file["pupils"]) {
            auto & pupil_data = _nodes.pupil(pupil_count).data();
            pupil_data.bundesland = parse_bundesland(pupil_json_data["Bundesland"]);
            pupil_data.input_file_id = pupil_json_data["Id"];
            pupil_data.grade = pupil_json_data["Klassenstufe"];
            for (auto const & fach : pupil_json_data["Fächer"]) {
                pupil_data.requested_subjects.emplace_back(parse_subject(fach));
            }
            pupil_count++;
        }
        _nodes.create_college_students(data_file["students"].size());
        unsigned student_count{0u};
        for (auto const student_json_data : data_file["students"]) {
            auto & student_data = _nodes.college_student(student_count).data();
            student_data.bundesland = parse_bundesland(student_json_data["Bundesland"]);
            student_data.input_file_id = student_json_data["Id"];
            for (auto const & offered_sub : student_json_data["Fächer"]) {
                Subject const subject = parse_subject(offered_sub["Fach"]);
                GradeRange range;
                for (auto const & grade: offered_sub["Klassenstufe"]) {
                    range.grades.push_back(grade);
                }
                student_data.offered_subjects.push_back({subject, 1., range});
            }
            student_count++;
        }

        //Create edges:
        create_edges();
        //Cache the edge costs:
        init_edge_costs();
    }

    void GraphCreator::init_edge_costs() {
        //Initialize the cost components that are used currently:
        edge_cost_computer.add_bundesland_bonus();
        edge_cost_computer.add_fachuebereinstimmung();
        //Iterate over all edges and cache the costs which are computed by the edge cost computer.
        for (auto & edge : _edges){
            edge.cost = edge_cost_computer.compute_edge_cost(
                    _nodes.college_student(edge.college_student_id), _nodes.pupil(edge.pupil_id));
        }
    }

    void GraphCreator::create_edges() {
        for (auto const & pupil : _nodes.pupils()){
            for (auto const & student : _nodes.college_students()) {
                if (pupil.accepts(student) and student.accepts(pupil)) {
                    _edges.emplace_back(pupil.id(), student.id());
                }
            }
        }
    }
}


